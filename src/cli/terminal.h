#pragma once

namespace angonoka::cli {
/**
    Make sure the application cleans up after itself when terminating.

    This includes showing TTY cursor if it was hidden.
*/
void register_abort_handlers();

/**
    Erase the current TTY line.
*/
void erase_line();

/**
    Move the TTY cursor up.
*/
void cursor_up();

/**
    Hide the TTY cursor.

    The cursor state will be restored if the application is
    terminated by SIGTERM or SIGINT.
*/
void hide_cursor();

/**
    Show the TTY cursor.
*/
void show_cursor();

/**
    Test if the output is a terminal.

    @return True if the output is a terminal.
*/
bool output_is_terminal() noexcept;
} // namespace angonoka::cli
