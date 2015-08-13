The following are average runtime of various operations over C = 30 times:
(for the unoptimized version, i.e. with assert and more-than-necessary copying)

N  = 1, i.e. plaintext length = 64
Encryption [average]: 7.86667e-05
Decryption [average]: 0.000348433
MMult_Hom [average]: 0.00114667
Xor_Hom [average]: 0.00430417
And_Hom [average]: 0.0044528

N = 2, i.e. plaintext length = 128
Encryption [average]: 0.000388233
Decryption [average]: 0.0014785
MMult_Hom [average]: 0.0048324
Xor_Hom [average]: 0.0187256
And_Hom [average]: 0.0191281

N = 3, i.e. plaintext length = 192
Encryption [average]: 0.0011246
Decryption [average]: 0.00464943
MMult_Hom [average]: 0.0154908
Xor_Hom [average]: 0.0508054
And_Hom [average]: 0.0522804

N = 4, i.e. plaintext length = 256
Encryption [average]: 0.00238137
Decryption [average]: 0.0072925
MMult_Hom [average]: 0.0239638
Xor_Hom [average]: 0.092868
And_Hom [average]: 0.0912833

To change C and N, simply do so in test_fhe.cpp. 