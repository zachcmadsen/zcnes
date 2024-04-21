fn main() {
    // Skip JAM, ANE, and LXA opcodes.
    const SKIP_OPC: [u8; 14] = [
        0x02, 0x12, 0x22, 0x32, 0x42, 0x52, 0x62, 0x72, 0x92, 0xB2, 0xD2, 0xF2, 0x8B, 0xAB,
    ];
    for opc in 0x00..=0xFF {
        if SKIP_OPC.contains(&opc) {
            continue;
        }

        println!("add_test(NAME ProcessorTests.{:02X} COMMAND tharte ${{CMAKE_SOURCE_DIR}}/../zcnes-tests/ProcessorTests/{:02x}.json)", opc, opc);
    }
}
