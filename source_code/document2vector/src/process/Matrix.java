package process;

import java.io.*;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;

/**
 * Created by linhong on 11/7/2016.
 */
public class Matrix {
    ArrayList<String> words;
    ArrayList<Integer> DF;
    HashMap<String, Integer> wordsIndex;
    int dnum;
    int maxtf;
    public Matrix(int _mtf){
        maxtf = _mtf;
    }
    public void InitDict(ArrayList<String> _words, ArrayList<Integer> _DF, HashMap<String,Integer> _wordsIndex) {
        words = _words;
        DF = _DF;
        wordsIndex = _wordsIndex;
    }
    public void readfile (String filename, int dID, BufferedWriter out) {
        try {
            FileReader fileReader = new FileReader(filename);
            // Always wrap FileReader in BufferedReader.
            BufferedReader bufferedReader = new BufferedReader(fileReader);
            String line;
            TokenProcessor tp =new TokenProcessor();
            HashMap<String,Integer> localdict = new HashMap<> (100);
            while ((line = bufferedReader.readLine()) != null) {
                ArrayList<String> res = tp.getTokenList(line);
                for(int i = 0; i < res.size(); i++) {
                    if (res.get(i).length() < 3) {
                        continue;
                    }
                    if (localdict.containsKey(res.get(i)) == false) {
                        localdict.put(res.get(i),1);
                    } else {
                        int ofq = localdict.get(res.get(i));
                        localdict.put(res.get(i),ofq+1);
                    }
                }
            }
            for (Map.Entry<String, Integer> entry : localdict.entrySet()){
                String w = entry.getKey();
                int widx = -1;
                if (this.wordsIndex.containsKey(w)) {
                    widx = this.wordsIndex.get(w);
                }
                if (widx == -1) {
                    continue;
                }
                int tf = entry.getValue();
                double score = (double)tf*Math.log((double)dnum/DF.get(widx)+1);
                score /= (double)maxtf;
                score *= dnum;
                if (score > 0.008) {
                    out.write(dID+"\t"+(widx+dnum)+"\t"+score+"\n");
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    public void readdir(String dir) {
        ArrayList<String> filenames = StrUtil.Initfolder(dir);
        this.dnum = filenames.size();
        FileWriter fis = null;
        try {
            fis = new FileWriter("Doc2word.txt");
            BufferedWriter out = new BufferedWriter(fis);
            for (int i = 0; i < filenames.size(); i++) {
                this.readfile(filenames.get(i),i,out);
            }
            out.close();
            System.out.println("number of nodes "+(dnum+wordsIndex.size()));
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    public void printdict(){
        FileWriter fis = null;
        try {
            fis = new FileWriter("words.txt");
            BufferedWriter out = new BufferedWriter(fis);
            for (int i = 0; i < words.size(); i++) {
                out.write(i+"\t"+words.get(i)+"\n");
            }
            out.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public static void main(String[] args) {
        if (args.length < 1) {
            System.out.println("Usage: [dirname] [pruned frequency (option)");
            System.exit(2);
        }
        ParseJson myparser = new ParseJson();
        myparser.readdir(args[0]);
        int freq = 20;
        if (args.length >1) {
            freq = Integer.parseInt(args[1]);
        }
        myparser.prune(freq);
        Matrix input = new Matrix(myparser.getMaxtf());
        input.InitDict(myparser.getwords(),myparser.getDF(),myparser.getWordsIndex());
        input.readdir(args[0]);
        input.printdict();
    }
}
