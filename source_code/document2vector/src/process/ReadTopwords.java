package process;

import util.WordPair;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.HashSet;
import java.util.PriorityQueue;
import java.util.Scanner;

/**
 * Created by linhong on 11/7/2016.
 */
public class ReadTopwords {
    int dimension;
    int dnum;
    int K;
    PriorityQueue<WordPair> []Q;
    String [] words;
    public ReadTopwords(int _K, int _dim, int _dnum) {
        K = _K;
        dnum = _dnum;
        dimension = _dim;
        Q = new PriorityQueue[dimension];
        for (int i = 0; i < Q.length; i++) {
            Q[i] = new PriorityQueue<WordPair>(K);
        }
    }
    public void parseEmbed(String filename) {
        try {
            FileReader fileReader = new FileReader(filename);
            // Always wrap FileReader in BufferedReader.
            BufferedReader bufferedReader = new BufferedReader(fileReader);
            String line;
            int linenum = 0;
            while ((line = bufferedReader.readLine()) != null) {
                linenum++;
                if (linenum < dnum) {
                    continue;
                }
                //System.out.println(line);
                Scanner sc = new Scanner(line);
                sc.useDelimiter(":");
                String w = sc.next();
                int pos = w.indexOf(","); //word index
                //System.out.println(w.substring(0,pos));
                int widx = Integer.parseInt(w.substring(0,pos));
                while (sc.hasNext()){
                    String s = sc.next();
                    pos = s.indexOf(",");
                    int idx = Integer.parseInt(s.substring(0,pos)); //dimension index
                    double weight = Double.parseDouble(s.substring(pos+1));
                    if (Q[idx].size() < K ) {
                        Q[idx].add(new WordPair(weight,widx-dnum));
                    } else {
                        WordPair o = Q[idx].peek();
                        if (weight > o.getWeight()) {
                            Q[idx].poll();
                            Q[idx].add(new WordPair(weight,widx-dnum));
                        }
                    }
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    public void readDict(String filename, int wordnum) {
        words = new String [wordnum];
        try {
            FileReader fileReader = new FileReader(filename);
            // Always wrap FileReader in BufferedReader.
            BufferedReader bufferedReader = new BufferedReader(fileReader);
            String line;
            while ((line = bufferedReader.readLine()) != null) {
                Scanner sc = new Scanner(line);
                sc.useDelimiter("\t");
                int idx = sc.nextInt();
                String w = sc.next();
                words[idx]  = w;
            }
            bufferedReader.close();
            fileReader.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    public void printTopword() {
        for (int i = 0; i < dimension; i++) {
            System.out.print("dimeionsion-"+i);
            while (!Q[i].isEmpty()) {
                WordPair o = Q[i].poll();
                System.out.print("\t"+words[o.getIdx()]+",");
                System.out.print(o.getWeight());
            }
            System.out.println();
        }
    }
    public static void main(String []args) {
        if (args.length < 1) {
            System.out.println("Usage: [K][dimension-d][#-of-documents] [#-of-words] [word-file] [embedding-file]");
            System.exit(3);
        }
        int K = 20;
        if (args.length > 0) {
            K = Integer.parseInt(args[0]);
        }
        int d = 20;
        if (args.length > 1) {
            d= Integer.parseInt(args[1]);
        }
        int doc =31;
        if (args.length > 2) {
            doc = Integer.parseInt(args[2]);
        } else {
            System.out.println("Usage: [K][dimension-d][#-of-documents] [#-of-words] [word-file] [embedding-file]");
            System.exit(3);
        }
        int numberword = 0 ;
        if (args.length > 3) {
            numberword = Integer.parseInt(args[3]);
        } else {
            System.out.println("Usage: [K][dimension-d][#-of-documents] [#-of-words] [word-file] [embedding-file]");
            System.exit(3);
        }
        String wordfile = null;
        String embedfile = null;
        if (args.length > 4) {
            wordfile = args[4];
        } else {
            System.out.println("Usage: [K][dimension-d][#-of-documents] [#-of-words] [word-file] [embedding-file]");
            System.exit(3);
        }
        if (args.length > 5) {
            embedfile = args[5];
        } else {
            System.out.println("Usage: [K][dimension-d][#-of-documents] [#-of-words] [word-file] [embedding-file]");
            System.exit(3);
        }
        ReadTopwords myreader = new ReadTopwords(K,d,doc);
        myreader.readDict(wordfile,numberword);
        System.out.println("finish reading words");
        myreader.parseEmbed(embedfile);
        myreader.printTopword();
    }
}
