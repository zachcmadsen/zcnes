namespace ZcNes.Core;

internal class Cpu
{
    private readonly ICpuBus bus;

    private ushort effectiveAddr;

    public byte A { get; set; }
    public byte X { get; set; }
    public byte Y { get; set; }
    public ushort Pc { get; set; }
    public byte S { get; set; }
    public byte P { get; set; }

    public Cpu(ICpuBus bus)
    {
        this.bus = bus;
    }

    public void Step()
    {
        var opc = bus.Read(Pc++);

        if (opc == 0xA5)
        {
            Zpg();
            Lda();
        }
    }

    private void Zpg()
    {
        effectiveAddr = bus.Read(Pc++);
    }

    private void Lda()
    {
        A = bus.Read(effectiveAddr);
        SetFlag(Flag.Zero, A == 0);
        SetFlag(Flag.Negative, (A & 0x80) != 0);
    }

    enum Flag
    {
        Carry = 0b00000001,
        Zero = 0b00000010,
        IrqDisable = 0b00000100,
        Decimal = 0b00001000,
        Break = 0b00010000,
        Push = 0b00100000,
        Overflow = 0b01000000,
        Negative = 0b10000000,
    }

    private void SetFlag(Flag flag, bool val)
    {
        if (val)
        {
            P |= (byte)flag;
        }
        else
        {
            P &= (byte)~flag;
        }
    }
}
