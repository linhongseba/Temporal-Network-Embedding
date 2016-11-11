package process;

import java.io.*;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;

/**
 * Created by linhong on 11/7/2016.
 */

public class ParseJson {
    ArrayList<Integer> DF;
    ArrayList<String> words;
    HashMap<String,Integer> wordsIndex;
    ArrayList<Integer> TFall;
    int wordnum;
    int maxtf;
    public ParseJson(){
        DF = new ArrayList<Integer>(10000);
        TFall = new ArrayList<Integer>(10000);
        wordsIndex = new HashMap<String,Integer>(10000);
        words = new ArrayList<String> (10000);
        wordnum = 0;
        maxtf = 0;
    }
    public void readfile (String filename) {
        try {
            FileReader fileReader = new FileReader(filename);
            // Always wrap FileReader in BufferedReader.
            BufferedReader bufferedReader = new BufferedReader(fileReader);
            String line;
            TokenProcessor tp =new TokenProcessor();
            HashSet<String> localdict = new HashSet<String> (100);
            while ((line = bufferedReader.readLine()) != null) {
                ArrayList<String> res = tp.getTokenList(line);
                for(int i = 0; i < res.size(); i++) {
                    if (res.get(i).length() <3) {
                        continue;
                    }
                    if (wordsIndex.containsKey(res.get(i)) == false) {
                        wordsIndex.put(res.get(i), wordnum);
                        TFall.add(wordnum,1);
                        DF.add(wordnum,1);
                        words.add(res.get(i));
                        localdict.add(res.get(i));
                        wordnum++;
                    } else {
                        int idx = wordsIndex.get(res.get(i));
                        int old = TFall.get(idx);
                        TFall.set(idx,old+1);
                        if (localdict.contains(res.get(i)) == false) {
                            int odf = DF.get(idx);
                            DF.set(idx,odf+1);
                            localdict.add(res.get(i));
                        }
                    }
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    public void prune(int freq){
        int i = 0;
        while (i < TFall.size()) {
            if (TFall.get(i) > maxtf) {
                maxtf = TFall.get(i);
            }
            if (TFall.get(i) <= freq) {
                TFall.remove(i);
                DF.remove(i);
                words.remove(i);
            } else {
                i++;
            }

        }
        wordsIndex.clear();
        for (i = 0; i < words.size();i++) {
            wordsIndex.put(words.get(i),i);
        }
    }
    public void print(){
        StrUtil.printMap(wordsIndex);
//        for(int i = 0; i < TFall.size();i++) {
//            System.out.println(words.get(i)+"\t"+TFall.get(i)+"\t"+DF.get(i));
//        }
    }
    public void readdir(String dir) {
        ArrayList<String> filenames = StrUtil.Initfolder(dir);
        for (int i = 0; i < filenames.size(); i++) {
            this.readfile(filenames.get(i));
        }
    }
    public ArrayList<Integer> getDF() {
        return this.DF;
    }
    public ArrayList<String> getwords(){
        return this.words;
    }
    public HashMap<String,Integer> getWordsIndex() {
        return this.wordsIndex;
    }
    public int getMaxtf() {
        return this.maxtf;
    }

    /*
    Unit Test
     */
    public static void main(String[] args){
        ParseJson myparser = new ParseJson();
        myparser.readdir("C:\\Users\\linhong\\Downloads\\doc2vec_input_GTD\\doc2vec_input_GTD");
        myparser.prune(10);
        //myparser.readfile("\\2ba5b65f-8b3e-4547-8e40-fd19fe73771f.json");
        myparser.print();
    }
}
