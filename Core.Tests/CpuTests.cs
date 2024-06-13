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
        
        List<ProcessorTest>? tests = JsonSerializer.Deserialize<List<ProcessorTest>>(jsonString, jsonSerializerOptions);
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

            cpu.Step();

            Assert.AreEqual(cpu.A, test.Final.A);
            Assert.AreEqual(cpu.X, test.Final.X);
            Assert.AreEqual(cpu.Y, test.Final.Y);
            Assert.AreEqual(cpu.Pc, test.Final.Pc);
            Assert.AreEqual(cpu.S, test.Final.S);
            Assert.AreEqual(cpu.P, test.Final.P);
            foreach (var ramState in test.Final.Ram)
            {
                Assert.AreEqual(ramState.Data, bus.Ram[ramState.Addr]);
            }
        }
    }
}