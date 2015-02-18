# RAW CAN message
Header header

# CAN Identifier
# 11-Bit-Identifier "Base frame format" (CAN 2.0A)
# 29-Bit-Identifier "Extended frame format" (CAN 2.0B)
uint32 identifier

# Flags
bool rtr
bool extended

# 0 to 8 data bytes
uint8[] data

