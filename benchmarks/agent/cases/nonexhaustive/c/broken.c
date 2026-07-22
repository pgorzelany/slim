enum Choice { YES, NO };
static int choose(enum Choice value) { switch (value) { case YES: return 0; } }
int main(void) { return choose(YES); }
