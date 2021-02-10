echo "lcs of test testtest: "
./sunum1cpp test testtest
./sunum1cpp test testtest a
./sunum1cpp test testtest a a
echo ""
echo "lcs of testtest testatest: "
./sunum1cpp testtest testatest
./sunum1cpp testtest testatest a
./sunum1cpp testtest testatest a a
echo ""
echo "lcs of testof taesaaaaaaaat: "
./sunum1cpp  testof taesaaaaaaaat
./sunum1cpp  testof taesaaaaaaaat a
./sunum1cpp  testof taesaaaaaaaat a a
echo ""
echo "lcs of testof taesaaaaaaaatof: "
./sunum1cpp testof taesaaaaaaaatof
./sunum1cpp testof taesaaaaaaaatof a
./sunum1cpp testof taesaaaaaaaatof a a
echo ""
echo "lcs of zxcfacdzfaczxzacxdadzcxfzdacxfzadcxfdcatestcdxfaxczfcxafzdxcfazcdx mlkpjomlpjklmjokmlpojlmkpojlkpmolkjpomkljpmolkjtestmlojpkomljpoklmjpolkmpjklmo: "
./sunum1cpp zxcfacdzfaczxzacxdadzcxfzdacxfzadcxfdcatestcdxfaxczfcxafzdxcfazcdx mlkpjomlpjklmjokmlpojlmkpojlkpmolkjpomkljpmolkjtestmlojpkomljpoklmjpolkmpjklmo 
./sunum1cpp zxcfacdzfaczxzacxdadzcxfzdacxfzadcxfdcatestcdxfaxczfcxafzdxcfazcdx mlkpjomlpjklmjokmlpojlmkpojlkpmolkjpomkljpmolkjtestmlojpkomljpoklmjpolkmpjklmo a a
echo ""
echo "lcs of testoftest taesaaaaaaaatasdptest: "
./sunum1cpp testoftest taesaaaaaaaatasdptest 
./sunum1cpp testoftest taesaaaaaaaatasdptest a a
echo ""
echo "lcs of two big strings which are same and size 4000"
 ./sunum1cpp  `echo "print 'a'*4000" | python` `echo "print 'a'*4000" | python` 
 ./sunum1cpp  `echo "print 'a'*4000" | python` `echo "print 'a'*4000" | python` a 
 ./sunum1cpp  `echo "print 'a'*4000" | python` `echo "print 'a'*4000" | python` a a a
 