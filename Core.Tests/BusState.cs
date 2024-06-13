using System.Text.Json.Serialization;

namespace ZcNes.Core.Tests;

[JsonConverter(typeof(BusStateJsonConverter))]
internal class BusState
{
    public ushort Addr { get; set; }
    public byte Data { get; set; }
    public CycleKind Kind { get; set; }
}
