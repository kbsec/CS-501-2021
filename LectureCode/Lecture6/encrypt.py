import os
from ctypes import create_string_buffer

def get_cstring(b):
    s = ""
    hex_b = b.hex()
    for i in range(len(hex_b)//2):
        s += f"\\x{hex_b[i * 2: i* 2 + 2]}"
    return f'"{s}"'

def xor_bytes(b0, b1):
    return bytes([i ^ j for i,j in zip(b0, b1)])



class EncryptedString:
    def __init__(self, data, max_size = 1000):
        # this wll just add a null byte to the string 
        self.data = create_string_buffer(data).raw
        self.key = os.urandom(len(self.data))
        self.ciphertext = xor_bytes(self.data, self.key)

    def getLiteral(self):
         
        #char buffer[5];auto z = xorString(buffer, x,y,5);
        literal_data = get_cstring(self.ciphertext)
        literal_key = get_cstring(self.key)
        l = len(self.data)
        ret = f"xorString({literal_data},{literal_key},{l});"
        return ret



def parse_header(header):
    new_config = []
    with open(header) as f:
        for line in f:
            if "#define" in line:
                try:
                    define, var, literal = line.split(" ")
                    es = EncryptedString(literal.encode("utf-8"))
                    new_config += [[define, var, es.getLiteral() ]]

                except Exception as e:
                    print(Exception, e)
                    continue
            
    header = ""
    for l in new_config:
        header += " ".join(l) + "\n"
    print("HEader")
    print(header)

if __name__ == "__main__":
    print("OK")
    parse_header("config.h")
    d = EncryptedString(b"asdf")
    #print(d.getLiteral())
    d = EncryptedString(b"super secret evil C2!")
    #print(d.getLiteral())
    # now its your turn! Finish the script to oeverwrite the header file!
    