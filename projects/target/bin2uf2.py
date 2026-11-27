# bin2uf2.py — convert a flash-base binary to UF2 (RP2040 family ID 0xe48bff56).
# UF2 spec: 512-byte blocks, magic 0x0A324655/0x9E5D5157/0x0AB16F30.
import struct, sys

def uf2_block(target_addr, payload, block_no, num_blocks, family=0xe48bff56):
    flags = 0x00002000  # familyID present
    return struct.pack(
        "<IIIIIIII",
        0x0A324655, 0x9E5D5157, flags, target_addr,
        len(payload), block_no, num_blocks, family,
    ) + payload.ljust(476, b"\x00") + struct.pack("<I", 0x0AB16F30)

def convert(src, dst, base):
    data = open(src, "rb").read()
    chunk = 256
    nblocks = (len(data) + chunk - 1) // chunk
    out = b"".join(
        uf2_block(base + i * chunk, data[i * chunk:(i + 1) * chunk], i, nblocks)
        for i in range(nblocks)
    )
    open(dst, "wb").write(out)

if __name__ == "__main__":
    convert(sys.argv[1], sys.argv[2], int(sys.argv[3], 0))
