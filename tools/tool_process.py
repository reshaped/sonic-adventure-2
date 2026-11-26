"""Run build tools in a process group of their own, with a time limit that
stops the whole group when it runs out."""

import contextlib
import math
import os
import shlex
import signal
import subprocess
import threading

from config import fail


UNITS = {'s': 1, 'm': 60, 'h': 60 * 60, 'd': 24 * 60 * 60}


def seconds(name, default):
    """Return the seconds environment variable name gives, or default where it
    is unset. The value is a number with an optional s, m, h or d suffix, as
    timeout(1) takes one. Zero gives None, no limit, as timeout(1) reads it."""
    given = os.environ.get(name)
    if given is None:
        return default
    number, unit = (given[:-1], given[-1]) if given[-1:] in UNITS else (given, 's')
    try:
        duration = float(number) * UNITS[unit]
        if math.isfinite(duration) and duration >= 0:
            return duration or None
    except ValueError:
        pass
    fail(f'{name}={given}: not a duration as timeout(1) reads one, such as 600 or 10m')


def stop(tool, grace):
    """Stop tool's whole process group: TERM, then KILL once its leader exits
    or grace seconds pass. A grace of None waits for the leader."""
    try:
        os.killpg(tool.pid, signal.SIGTERM)
        tool.wait(grace)
    except (ProcessLookupError, subprocess.TimeoutExpired):
        pass
    finally:
        with contextlib.suppress(ProcessLookupError):
            os.killpg(tool.pid, signal.SIGKILL)


@contextlib.contextmanager
def interrupted():
    """Allow an ordinary termination signal to unwind the child's cleanup."""
    def terminate(number, _frame):
        raise SystemExit(128 + number)

    saved = {}
    try:
        if threading.current_thread() is threading.main_thread():
            for number in (signal.SIGHUP, signal.SIGQUIT, signal.SIGTERM):
                if signal.getsignal(number) == signal.SIG_DFL:
                    saved[number] = signal.signal(number, terminate)
        yield
    finally:
        for number, handler in saved.items():
            signal.signal(number, handler)


def run(argv, *, timeout=None, check=False, input=None, **options):
    """Run argv once, preserving its streams and exit status. The limit lasts
    until argv exits and its pipes close, so a helper left running after that
    is not stopped. A timeout stops the whole group and is fatal."""
    limit = seconds('HUNG', 600) if timeout is None else timeout
    grace = seconds('GRACE', 10)
    if input is not None:
        if options.get('stdin') is not None:
            raise ValueError('stdin and input cannot both be supplied')
        options['stdin'] = subprocess.PIPE
    with interrupted(), subprocess.Popen(argv, start_new_session=True, **options) as tool:
        try:
            stdout, stderr = tool.communicate(input, timeout=limit)
        except subprocess.TimeoutExpired as error:
            stop(tool, grace)
            command = shlex.join(map(str, argv))
            messages = [f'{command}: timed out after {limit:g} seconds']
            # A binary result (e.g. git show) can be large. Only include diagnostic text.
            detail = error.stderr or (error.stdout if options.get('stderr') == subprocess.STDOUT else None)
            if detail:
                messages.append(detail.decode(errors='replace').rstrip())
            fail(*messages)
        except BaseException:
            stop(tool, grace)
            raise
    result = subprocess.CompletedProcess(argv, tool.returncode, stdout, stderr)
    if check:
        result.check_returncode()
    return result
