package util;

/**
 * Created by linhong on 11/7/2016.
 */
public class WordPair extends Pair implements Comparable{

    public WordPair(double key, int value) {
        // key: the weight of words
        // value: the idx of words
        super(key, value);
    }

    public void setKey(double key) {
        super.setKey(key);
    }
    public void setValue(int value){
        super.setValue(value);
    }
    public double getWeight() {
        return ((Double)super.getKey()).doubleValue();
    }
    public int getIdx(){
        return ((Integer)super.getValue()).intValue();
    }
    @Override
    public int compareTo(Object o) {
        return compareTo((WordPair)o);
    }
    public int compareTo(WordPair o){
        return Double.compare(getWeight(),o.getWeight());
    }
}
