filename=$(basename $2)
name=${filename%.*}
./$1 $2 -json -astonly > $name.json
python3.6 json_to_dot.py $name.json > $name.dot
dot -Tpng $name.dot -o $name.png
rm $name.json
rm $name.dot