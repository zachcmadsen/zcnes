using System.Text.Json;
using System.Text.Json.Serialization;

namespace ZcNes.Core.Tests;

internal class BusStateJsonConverter : JsonConverter<BusState>
{
    public override BusState? Read(
        ref Utf8JsonReader reader,
        Type typeToConvert,
        JsonSerializerOptions options
    )
    {
        if (reader.TokenType != JsonTokenType.StartArray)
        {
            throw new JsonException();
        }

        var busState = new BusState();

        reader.Read();
        busState.Addr = reader.GetUInt16();

        reader.Read();
        busState.Data = reader.GetByte();

        reader.Read();
        busState.Kind = reader.GetString() switch
        {
            "read" => CycleKind.Read,
            "write" => CycleKind.Write,
            _ => throw new JsonException()
        };

        reader.Read();
        if (reader.TokenType != JsonTokenType.EndArray)
        {
            throw new JsonException();
        }

        return busState;
    }

    public override void Write(Utf8JsonWriter writer, BusState value, JsonSerializerOptions options)
    {
        throw new NotImplementedException();
    }
}
