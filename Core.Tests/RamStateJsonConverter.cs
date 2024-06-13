using System.Text.Json;
using System.Text.Json.Serialization;

namespace ZcNes.Core.Tests;

internal class RamStateJsonConverter : JsonConverter<RamState>
{
    public override RamState? Read(
        ref Utf8JsonReader reader,
        Type typeToConvert,
        JsonSerializerOptions options
    )
    {
        if (reader.TokenType != JsonTokenType.StartArray)
        {
            throw new JsonException();
        }

        var ramState = new RamState();

        reader.Read();
        ramState.Addr = reader.GetUInt16();

        reader.Read();
        ramState.Data = reader.GetByte();

        reader.Read();
        if (reader.TokenType != JsonTokenType.EndArray)
        {
            throw new JsonException();
        }

        return ramState;
    }

    public override void Write(Utf8JsonWriter writer, RamState value, JsonSerializerOptions options)
    {
        throw new NotImplementedException();
    }
}
