// TODO: See pacman for masking log levels.
enum zc_log_level { zc_log_error };

void zc_log(enum zc_log_level level, const char *fmt, ...);
