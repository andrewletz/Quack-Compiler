filename=$(basename $1)
name=${filename%.*}
./qcc $1 -json -astonly > $name.json
python3 json_to_dot.py $name.json > $name.dot
dot -Tpng $name.dot -o $name.png
rm $name.json
rm $name.dot