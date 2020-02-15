for f in *.o; do
  echo $f
  ./$f
done