def greycode(n):
    '''
    Generates binary refected grey codes for n bits/switches
    '''
    if n == 1:  # Base Case
        return ["0", "1"]           # Only possible combinations
    
    # Subsequent Cases
    last_greycode = greycode(n - 1)
    last_greycode_flipped = last_greycode[::-1]
    last_greycode_plus0 = list(map(lambda x: f'0{x}', last_greycode))
    last_greycode_flipped_plus1 = list(map(lambda x: f'1{x}', last_greycode_flipped))

    return last_greycode_plus0 + last_greycode_flipped_plus1

#print(greycode(3))

def greycode_but_binary(n):
    '''
    Generates binary refected grey codes for n bits/switches
    '''
    if n == 1:  # Base Case
        return ["0", "1"]           # Only possible combinations
    
    # Subsequent Cases
    L1 = greycode_but_binary(n - 1)
    L2 = L1                         # This is just a temp variable now because we're about to overwrite L1
    L1 = list(map(lambda x: f'0{x}', L1))
    L2 = list(map(lambda x: f'1{x}', L2))

    return L1 + L2

print(greycode_but_binary(3))