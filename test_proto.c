int x;

void setglobalx(void)
{
    x = 10;
}

void printglobalx(void)
{
    println(x);
}

void foo(int x)
{
    println(x);
    {
        int x;
        x = 2000;
        {
            int x;
            x = 3000;
        }
        println(x);
    }
    println(x);
}

void main(void)
{
    int x;

    setglobalx();
    x = 100;
    printglobalx();

    foo(1000);

    println(x);
    printglobalx();
}