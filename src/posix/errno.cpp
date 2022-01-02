int _errno = 0;

int get_errno() { return _errno; }
void set_errno(int e) { _errno = e; }