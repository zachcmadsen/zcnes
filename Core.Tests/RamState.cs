using System.Text.Json.Serialization;

namespace ZcNes.Core.Tests;

[JsonConverter(typeof(RamStateJsonConverter))]
internal class RamState
{
    public ushort Addr { get; set; }
    public byte Data { get; set; }
}
