namespace ZcNes.Core.Tests;

internal class ProcessorTest
{
    public required string Name { get; set; }
    public required CpuState Initial { get; set; }
    public required CpuState Final { get; set; }
    public required List<BusState> Cycles { get; set; }
}
