using CommunityToolkit.HighPerformance;
using CommunityToolkit.HighPerformance.Helpers;

namespace ZcNes.Core;

internal class Cpu
{
    // Status flags
    private bool carry;
    private bool zero;
    private bool interruptDisable;
    private bool dec;
    private bool b;
    private bool overflow;
    private bool negative;

    private ushort effectiveAddr;

    private readonly ICpuBus bus;

    public byte A { get; set; }
    public byte X { get; set; }
    public byte Y { get; set; }
    public ushort Pc { get; set; }
    public byte S { get; set; }
    public byte P
    {
        get
        {
            return (byte)(
                carry.ToByte()
                | zero.ToByte() << 1
                | interruptDisable.ToByte() << 2
                | dec.ToByte() << 3
                | b.ToByte() << 4
                | 1 << 5
                | overflow.ToByte() << 6
                | negative.ToByte() << 7
            );
        }
        set
        {
            carry = BitHelper.HasFlag(value, 0);
            zero = BitHelper.HasFlag(value, 1);
            interruptDisable = BitHelper.HasFlag(value, 2);
            dec = BitHelper.HasFlag(value, 3);
            b = BitHelper.HasFlag(value, 4);
            overflow = BitHelper.HasFlag(value, 6);
            negative = BitHelper.HasFlag(value, 7);
        }
    }

    public Cpu(ICpuBus bus)
    {
        carry = false;
        zero = false;
        interruptDisable = true;
        dec = false;
        b = true;
        overflow = false;
        negative = false;

        effectiveAddr = 0;

        this.bus = bus;

        A = 0;
        X = 0;
        Y = 0;
        Pc = 0;
        S = 0xFD;
    }

    public void Step()
    {
        var opc = bus.Read(Pc++);
        Zpg();
        Lda();
    }

    private void Zpg()
    {
        effectiveAddr = bus.Read(Pc++);
    }

    private void Lda()
    {
        A = bus.Read(effectiveAddr);
        zero = A == 0;
        negative = BitHelper.HasFlag(A, 7);
    }
}
