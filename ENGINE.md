# Engine

[`engine/dc/`](engine/dc) holds the C the Dreamcast port adds or uses in place of shipped assembly. The matching build doesn't compile it. [Build model](PORTS.md#build-model) describes how the GCC build selects the objects for each image.

## Replacements

If an image uses one of these four assembly files in the matching build, the port links its C replacement instead.

| Shipped assembly | Port C | Functions |
|---|---|---|
| `sdk/ninja/njDirectPointLoops.src` | [`direct_points.c`](engine/dc/direct_points.c) | `njDirectCalcPointLoop` and `njDirectCalcPointLoopD8` |
| `sdk/ninja/njLine3DKernels.src` | [`line.c`](engine/dc/line.c) | `njCalcPoint3DLine`, `njCalcLine3D` and `njCalcLine3DClip` |
| `sdk/runtime/__strcmp.src` | [`strcmp.c`](engine/dc/strcmp.c) | `_slow_strcmp1` and `_quick_strcmp1` |
| `sdk/ninja/njTexMemCopy.src` | [`texture_copy.c`](engine/dc/texture_copy.c) | `njTexMemCopy` and its 32-, 16- and 8-byte copies |

The shipped point and line drawers use `BSR` to call a near clipper. GCC can place the clipper beyond the branch's reach of about 4 KB in either direction. The shipped D8 loop also overwrites R8 without saving it. Both C point loops skip strips of fewer than three records.

The point loops use one `FTRV` per corner, with w set to 1 or taken from the D8 record. The line drawer uses SH4ZAM for `FTRV` and `FSRRA` and keeps the operand and instruction order of the other float operations. With matching FPSCR settings, both C drawers pass on the same float bits as the shipped assembly.

Both string routines compare bytes as signed chars. The C reads one byte at a time, so even the quick comparison stops at the terminator.

The shipped `njTexMemCopy` falls through into the branch at the start of SHC's `njTexScalar.c`. The replacement calls the copy it selects. Each copy does nothing for a size below its width.

## Kernels

The main executable adds two files of SH-4 matrix kernels. Each has the assembly and its C model.

When XMTRX holds the matrix, [`njCalcPoints`](sdk/ninja/njCalcPoints.c) can use [`xmtrx.c`](engine/dc/xmtrx.c) to transform points in pairs. The kernel pipelines two `FTRV`s and reads exactly the requested count. With an explicit matrix `njCalcPoints` works forward one point at a time. It does the same if writing a result would overwrite a later input.

[`turn.c`](engine/dc/turn.c) replaces an uninterrupted run of matrix calls: push the unit matrix, move it if the shape has a move, turn it, transform one vector or point, and pop. There can be no intervening statement, second read or branch. The kernel for that shape works in registers, leaves XMTRX and the stack alone, and gives the same bits for finite vectors.

Include `"engine/dc/turn.h"` after the other includes under `#ifdef NONMATCHING`. Keep the whole push-to-pop sequence in the matching branch. The other branch is one kernel call with the same operands: the move vector if any, angles in x, y, z order, and the input and output vectors by their first floats. [`turn_to_world`](src/player/player.c) is an example; [`tools/windows.py`](tools/windows.py) lists the supported shapes. Chunk nodes still use [Ninja's translation, turn and scale](PORTS.md#a-nodes-translation-turn-and-scale).

## Checked loading

**PRS.** [`prs_decompress_checked`](engine/dc/prs_checked.c) checks the whole stream before writing. The end marker must be inside the input, a copy must not reach back before the output buffer, and the decoded bytes must fit. It also rejects output that overlaps the stream. Downloaded karts use this decoder.

[`loader_read_prs_buffer`](src/system/loader.c) reads a file into the end of its destination and calls `prs_decompress_in_place`. `PRS_INVALID` fails the load. `PRS_OVERRUN` means the stream is valid but decoding would overwrite unread input. In either case the decoder leaves the buffer untouched. Only on `PRS_OVERRUN` does the loader retry from a heap copy.

**Textures.** The [PVM loader](engine/dc/pvm.c) checks the directory, chunk bounds and texture count before Ninja opens an archive. Each texture must have a format, dimensions and palette Ninja accepts. Some mipmaps on the disc end 4 or 10 bytes short of the surface Ninja copies. `texture_load_pvm_memory` pads those chunks with zeros in a heap copy. `texture_load_pvm_buffer` pads them in the load buffer if there's room.

Ninja loads into a temporary list. The destination list gets its handles only once every texture has loaded. The loader records the handles by list address so [`njReleaseTexture`](sdk/ninja/njReleaseTexture.c) can release them even after another overlay replaces the list. A failed load releases the recorded handles and clears the destination list's handles.

**Downloaded karts.** [`cart_chunk_streams_valid`](engine/dc/cart_chunk.c) checks the vertex and polygon lists before [`CART.PRS` relocates the model's offsets](modules/cart/model_archive.c). It accepts the chunks `njCnkSimpleVlist` and `njCnkSimplePlist` can draw. Every read must stay inside the model, and a strip can name only vertices the vertex list writes.

**File sizes and lifetimes.** A bounded load fails if the file won't fit its buffer. The loader waits for the whole GD request and copies anything it needs to keep before reusing the scratch buffer. It writes back a decoded overlay and invalidates the instruction cache before calling its entry.

A Chao area's callbacks expire when the next area replaces it. The model and motion archives stay live with the parent. Each archive must fill its reserved extent. The [camera loader](src/system/loader.c) checks the camera parts before copying them into the 64 KiB common bank and four 16 KiB player banks.

## Features

The timing readout, 16:9 and universal 60 fps are off at boot. [`port.c`](engine/dc/port.c) handles their controls. 16:9 and universal 60 fps are both persisted on unused bits of `save_data.status`: `0x10` and `0x20`. The treasure radar and the extra cutscene skips have no toggle.

| Press, on the first pad | Does |
|---|---|
| Hold L and R, then up | Steps the readout |
| Hold L and R, then right | Turns 16:9 on or off |
| Hold L and R, then down | Turns universal 60 fps on or off |
| Start, in a story event or a stage's opening scene | Skips it, with the [exceptions below](#skips) |

A chord's effect appears on screen for two seconds. The game doesn't see L, R or d-pad input from the press until all are up.

### Universal 60 fps

Most of the game already runs at 60 fps: the menus, one-player stages, bosses, the Chao world and most story events. Two-player games and the events that ask for half rate run at 30. Universal 60 fps brings those up to 60 too.

A [simulation step](src/system/frame.h) advances play by one vertical blank's time. The game sets a frame interval in blanks (`frame_interval`) and a step count (`frame_step_count`). On a 60 Hz display the shipped rates are:

| Where | Interval | Steps | Pictures per second |
|---|---:|---:|---:|
| Title, menus, kart menus, emblem and ending | 1 | 1 | 60 |
| One-player stage, boss, Chao world or story kart | 1 | 1 to 4 | 60, 30, 20 or 15 |
| Two-player stage or kart race | 2 | 2 to 4 | 30 or less |
| Event whose first sound cue asks for half rate | 2 | 2 | 30 |
| Other events | 1 | 1 | 60 |

`stage_run` chooses the step count from the last frame's timings. A stage that takes two blanks to draw can take two steps next frame and keep its speed.

With universal 60 fps on, [`pace.c`](engine/dc/pace.c) can show two pictures in a two-blank frame with two steps. Instead of drawing once after both steps, it shows each step at its own blank. Fades and saved play time still advance at the game's rate.

After the extra draw the port restores Ninja's view and the state of `rand()`. The game's frame timer stands still during that draw. The second step samples the pads one blank later. Effects timed by draws, such as the score popup, advance with each picture.

Stage frames split only in `STAGE_PLAY`, outside demos. Splitting starts when three held frames in a row could each have drawn both pictures with a fourteenth of a blank to spare. If two split frames run late within 30 frames of each other, the port returns to one picture. It waits 30 frames before trying again and doubles the wait after each stop, up to 480 frames. The readout shows `HELD` when a held frame draws once with the toggle on.

### 16:9

The Dreamcast still draws 640 by 480 pixels. [`wide.c`](engine/dc/wide.c) draws a view a third wider so a 16:9 display stretches it back to shape. Ninja's horizontal aspect (`_nj_scr_aspect_x_`) goes from 1 to 3/4. The view keeps its height. Positions the game projects for itself stay in 4:3 screen coordinates.

During play, objects in the wider view count as on screen. Demos keep the 4:3 window when playing or recording. Changing which objects are in view can change the calls to `rand()` and put a replay out of step. Black bands cover the sides during demos. Land and sphere culling follow the picture's width.

A 2D draw squeezes horizontally about the center of its view to keep its shape. Each player in split screen has a separate center. One-player counters stay anchored to the left edge. Fades cover the full screen, and movies play between black bands. A full-screen textured picture stays centered with the scene visible at the sides. Debug text and the readout stretch with the display.

### The radar

The [treasure radar](src/player/knuckles.c) follows all three pieces, as in the first game. In the retail game, the radar would only follow one item at a time. Each remaining piece has a gauge that shows its distance. In two-player hunts each player has the gauges on their own screen.

The nearest piece sets the beep rate: every 64 steps within 500, 32 within 300 and 16 within 100. The hunter speaks once for each piece within 100. Taking a piece resets the voice cues. Hints still follow the game's original piece order. Time and hints still determine the bonus.

### Skips

Start skips movies and most story events, even on first viewing. The hero story's finale and the three staff-roll events keep playing. The port also takes Start during an event's opening typed line.

[Fifteen story stages](src/system/sequence.c), including boss fights, open with a mini event. Start takes the scene and camera to the step before their last. The game runs the final step itself, puts the players in place and starts the stage or fight. A skip plays the last music cue it passes, so the stage gets its music. It closes the text window and stops the voices without playing the skipped scene's other cues.

### The readout

The first press shows pictures per second at the top right, the second adds the times page, and the third turns the [readout](engine/dc/readout.c) off. The rate counts pictures over at least the last half second. A split frame counts as two pictures. At one picture per blank on a 50 Hz display it reads 50; outside PAL it counts 60 blanks per second. A frame that runs at least a second late restarts the count. Shorter stalls count against the rate. The readout stays off during movies and doesn't count their pictures.

The times page shows the modes, frame time, steps per frame, draw time and render time. It also counts frames that used up their blanks before `frame_wait`. The color of the frame time shows how its peak compares with the time available.

### The splash

`OTHERPRINT.PRS` shows a SH4ZAM splash after the Sonic Team logo. It fades in, holds for 120 steps and fades out. Start ends the fade-in or the hold early.

[`sh4zam_splash.c`](engine/dc/sh4zam_splash.c) supplies the picture and [`logos.c`](modules/otherprint/logos.c) runs the sequence. The overlay loads the picture through the checked PVM loader and releases its texture after the fade or when the logo task ends. If the picture can't be allocated or loaded, the logos continue.

## Crash screen

[`crash.c`](engine/dc/crash.c) writes straight into the displayed frame buffer, independently of Kamui, Ninja and the game's drawing code.

### Stops

An address error, a `TRAPA` or an illegal instruction stops the game with a report. `port_stop` uses the same screen when the game can't continue. If a required file fails to load, the screen names it. A failed read that the drive won't end also stops the game.

An [exception report](engine/dc/crash_report.h) shows the PC, PR, SR, stack pointer and R0 to R14. An address error gives the fault address; `TRAPA` gives the trap number. `port_stop` leaves out PR and R0 to R14. The screen shows the game mode, stage and name in the stage slot, with `?` for an unrecognized name.

The caller list shows up to 24 possible return addresses from the first 512 stack words in main RAM. A return address can remain on the stack after its function returns.

The resident and `MCWARN.PRS` each install their own handlers and restore the old ones on exit. The memory card warning can give a report even under a retail resident.

### Hangs

The [hang watch](engine/dc/crash_watch.c) counts consecutive vblank callbacks with the same scanout address (`FB_R_SOF1`). After 1,800 it draws a report: 30 seconds at 60 Hz or 36 at 50 Hz. The game keeps running. The watch redraws the report every 30 callbacks until the address changes.

The watch reads Shinobi's outermost interrupt frame to find where the game was interrupted. If a fiber switch is pending, it uses the PC and SR saved before the switch. `CODE UNDER A HANDLER WHOSE PC IS NOT KNOWN` means the vertical blank interrupted another handler. The reported PC belongs to the code underneath it. Without a separate interrupt stack the screen says `PC SR AND SP NOT KNOWN` and gives the mode, stage and slot.

### Limits

The exception report uses fixed storage and its own stack. If the report faults, `REPORT FAULTED` gives the second exception's code and PC below what remains of the first report. The screen draws only if the report fits in video memory at the 640-pixel scanout layout.

## Tests

[`test_windows.py`](tests/test_windows.py) checks the guarded calls against their windows. [`test_kernels.py`](tests/test_kernels.py) compares the windows with the listings, the C models with the shipped instructions, and the compiled kernels with their models. [`test_zam.py`](tests/test_zam.py) checks Ninja helpers against instruction references in the SH-4 model.

| Tests | What they check |
|---|---|
| [`test_loader.py`](tests/test_loader.py), [`test_system.py`](tests/test_system.py), [`test_cart.py`](tests/test_cart.py) | The checked loaders: files, PRS streams, textures and downloaded karts |
| [`test_port.py`](tests/test_port.py) | The chord and its toggles, the save's two bits, the split frame, the readout's rate and 16:9 |
| [`test_hunt_radar.py`](tests/test_hunt_radar.py), [`test_event_typing.py`](tests/test_event_typing.py), [`test_sh4zam_splash.py`](tests/test_sh4zam_splash.py) | The radar, the end of an event's typed line and the splash's archive |
| [`test_crash.py`](tests/test_crash.py) | The crash report, the handlers' vectors and the hang watch |

See [Checks](CONTRIBUTING.md#checks) for what to run on a change.
