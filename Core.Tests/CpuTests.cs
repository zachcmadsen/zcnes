using System.Text.Json;

namespace ZcNes.Core.Tests;

[TestClass]
public class CpuTests
{
    private static readonly JsonSerializerOptions jsonSerializerOptions = new JsonSerializerOptions
    {
        PropertyNameCaseInsensitive = true
    };

    [TestMethod]
    [DataRow("a5")]
    public void ProcessorTest(string opc)
    {
        string jsonString = File.ReadAllText($"{opc}.json");

        List<ProcessorTest>? tests = JsonSerializer.Deserialize<List<ProcessorTest>>(
            jsonString,
            jsonSerializerOptions
        );
        Assert.IsNotNull(tests);
        Assert.AreEqual(10_000, tests.Count);

        var bus = new ProcessorTestCpuBus();
        var cpu = new Cpu(bus);

        foreach (var test in tests)
        {
            cpu.A = test.Initial.A;
            cpu.X = test.Initial.X;
            cpu.Y = test.Initial.Y;
            cpu.Pc = test.Initial.Pc;
            cpu.S = test.Initial.S;
            cpu.P = test.Initial.P;
            foreach (var ramState in test.Initial.Ram)
            {
                bus.Ram[ramState.Addr] = ramState.Data;
            }
            bus.Cycles.Clear();

            cpu.Step();

            Assert.AreEqual(test.Final.A, cpu.A);
            Assert.AreEqual(test.Final.X, cpu.X);
            Assert.AreEqual(test.Final.Y, cpu.Y);
            Assert.AreEqual(test.Final.Pc, cpu.Pc);
            Assert.AreEqual(test.Final.S, cpu.S);
            Assert.AreEqual(test.Final.P, cpu.P);
            foreach (var ramState in test.Final.Ram)
            {
                Assert.AreEqual(ramState.Data, bus.Ram[ramState.Addr]);
            }
            Assert.AreEqual(test.Cycles.Count, bus.Cycles.Count);
            for (int i = 0; i < test.Cycles.Count; ++i)
            {
                Assert.AreEqual(test.Cycles[i].Addr, bus.Cycles[i].Addr);
                Assert.AreEqual(test.Cycles[i].Data, bus.Cycles[i].Data);
                Assert.AreEqual(test.Cycles[i].Kind, bus.Cycles[i].Kind);
            }
        }
    }
}
