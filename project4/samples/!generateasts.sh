for f in *; do
  if [[ $f == *.qk ]] # if its a quack file
  then
    filename=$f
    name=${filename%.*}
    ../qcc $filename --json=true > $name.json
    python3 ../json_to_dot.py $name.json > $name.dot
    dot -Tpng $name.dot -o $name.png
    rm $name.json
    rm $name.dot
  fi
done;

