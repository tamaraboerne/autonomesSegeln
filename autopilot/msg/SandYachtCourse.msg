uint64 timestamp # without it doesnt compile

uint8 length
# since uorb can not handle dynamic arrays, we use fixed size arrays and fill it with 0.
# 32 units should be far enough.
int16[32] x
int16[32] y
uint8[32] type
