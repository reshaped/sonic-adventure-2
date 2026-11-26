/* a message's page as one sprite: its lines from message up to the next page
 * break, joined by newlines. messages.c includes this twice, for the dialog
 * and the service page, whose sprite factories set different text pitches.
 * PAGE_FUNCTION names each copy, and PAGE_FACTORY the factory it calls */
static struct text_sprite *PAGE_FUNCTION(int message, Sint8 bank, Uint16 *width,
                                         Uint16 *height)
{
    int first;
    int entry;
    int length;
    int done;
    int count;
    int bytes;
    int next;
    char *page;
    const char *text;
    int part;
    struct text_sprite *sprite;

    length = 0;
    done = FALSE;
    if (message < 0) {
        return NULL;
    }
#ifdef NONMATCHING
#else
    /* @hack redundant: the height operand before the first entry
     * orders the saved registers in both versions of this loop. */
    if (0) {
        if (height) {
        }
    }
#endif
    first = message;
    entry = first;
    count = 0;
    do {
#ifdef NONMATCHING
        /* this side repairs the bug tagged in the matching side. each line
         * after the first counts its separator. it returns null where a line
         * has no text */
        text = kinder_message_text(entry, bank);
        if (!text) {
            return NULL;
        }
        bytes = strlen(text);
        length += bytes;
        if (count) {
            length++;
        }
#else
        /* @bug memory: empty lines still get separators at 0x0c9ca390
         * and 0x0c9cb2d8. Counting only nonempty lines can underallocate. */
        bytes = strlen(kinder_message_text(entry, bank));
        length += bytes;
        if (bytes > 0) {
            length++;
        }
#endif
        count++;
        next = kinder_message_next_line(entry, bank);
        if (next < 0 || next == KINDER_MESSAGE_PAGE) {
            done = TRUE;
        } else {
            entry = next;
        }
    } while (!done);
    length++;
    page = syCalloc(1, length);
    if (!page) {
        return NULL;
    }
    for (part = 0, entry = first; part < count; part++) {
        text = kinder_message_text(entry, bank);
        if (part > 0 && text) {
            strcat(page, "\n");
        }
        strcat(page, text);
        next = kinder_message_next_line(entry, bank);
        if (next < 0 || next == KINDER_MESSAGE_PAGE) {
            break;
        }
        entry = next;
    }
    sprite = PAGE_FACTORY(page, width, height);
    syFree(page);
    return sprite;
}
#undef PAGE_FUNCTION
#undef PAGE_FACTORY
