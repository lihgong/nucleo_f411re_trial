import serial

com_port = 'COM19'
baud_rate = 115200
ser = serial.Serial(com_port, baud_rate, timeout=0.1)

def decode(buf):
    # find the 1st 0x55 in buf. If none, the buffer can be dropped
    idx = buf.find(0x55)
    if idx == -1:
        return False, len(buf), 0, 0 # status, consumed_buf, dec_start, dec_len
    else:
        # check whether we had sufficient buffer (maybe they're not arrived yet)
        if len(buf) <= idx+1:
            return False, 0, 0, 0 # status, consumed_buf, dec_start, dec_len
        pkt_len = buf[idx+1] #1 len
        # check whether the packet arrived
        if len(buf) <= idx+pkt_len+4-1:
            return False, 0, 0, 0 # status, consumed_buf, dec_start, dec_len
        pkt_crc = buf[idx+pkt_len + 3]
        crc_calc = 0
        for x in range(0, pkt_len + 3):
            crc_calc += buf[idx+x]
        crc_calc &= 0xFF
        return True, pkt_len+4, idx+3, pkt_len # status, consumed_buf, dec_start, dec_len



data_buf = b''
try:
    while True:
        while ser.in_waiting: # data is pending
            data_buf += ser.read(4096)

            #print(len(data_buf))
            #print(data_buf)
            #for idx,x in enumerate(data_buf):
            #    print('idx=%d %02X' % (idx, x))
            #data_buf = b''


            while True:
                status, consumed_buf, start, dec_len = decode(data_buf)
                #print(status, consumed_buf, start, dec_len)
                if status == True: # find the frame
                    data = data_buf[start:start+dec_len]
                    print('Decoded data: %s' % data)
                data_buf = data_buf[consumed_buf:]
                if status == False:
                    break # end of decode
            #print(data_buf) # remain buf



except KeyboardInterrupt:
    ser.close()

