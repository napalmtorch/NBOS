using system;

def hexval  = 0xDEADC0DE;
def decval  = 1234;
def boolval = true;
def charval = 'X';
def strval  = "TESTING 1234 xD";

class rectangle
{
    def x      = 0;
    def y      = 0;
    def width  = 0;
    def height = 0;

    func contains(xx, yy) { return (xx >= x && yy >= y && xx < x + width && yy < y + height); }
}

func do_math(x, y)
{
    return x + y * (10 + x) / ((x * y) + 2);
}

func main()
{
    printf("Hello world!\n");
    def num = do_math(10, 25);

    def rect = rectangle();
    def rect2 = rectangle(32, 32, 320, 240);

    rect2.x      += 8;
    rect2.y      -= 8;
    rect2.width  = 400;
    rect2.height = 300;

    if (rect2.contains(64, 64)) { printf("rectangle contains"); }
    else { printf("rectangle doesn't contain lol RIP"); }

    if (num >= 300) { printf("big number\n"); }
    else { printf("small number\n"); }

    def i = 0;
    while (true)
    {
        if (i >= 69) { break; }
        i++;
    }

    def arr[64] = { 0 };
    for (i = 0; i < 420; i += 3) { printf("A NUM %d", i); }

    printf("Program is finished now...\n");
    return 0;
}