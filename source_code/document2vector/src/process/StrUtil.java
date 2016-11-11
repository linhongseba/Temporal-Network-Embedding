package process;


import java.io.BufferedWriter;
import java.io.File;
import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Date;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author Linhong
 * @email: linhong.seba.zhu@gmail.com
 */

public class StrUtil {
    private static Set<Integer> windowsCharset;

	
    /**
     *
     * @param str
     * @return
     */
    public static boolean isnumric (String str){
        return str.matches("-?\\d+(\\.\\d+)?");
    }
    public static int GetMapMaxvalue(Map mp){
        int max=0;
        Iterator it = mp.entrySet().iterator();
        while(it.hasNext()){
            Map.Entry pairs=(Map.Entry)it.next();
            Integer w=(Integer)pairs.getValue();
            if(w>max)
                max=w;
        }
        return max;
    }
    public static String fixEncoding(String latin1) {
            try{
                byte[] bytes = latin1.getBytes("ISO-8859-1");
                if (!validUTF8(bytes))
                    return latin1;   
                return new String(bytes, "UTF-8");  
            } catch (Exception e) {
                // Impossible, throw unchecked
                throw new IllegalStateException("No Latin1 or UTF-8: " + e.getMessage());
            }
    }
            
      	public static ArrayList<String> Initfolder(String path){
             ArrayList<String> filelists=new ArrayList<String>(100);
            try {
                LinkedList<String> Dir = new LinkedList<String>();
                File f = new File(path);
                Dir.add(f.getCanonicalPath());
                while(!Dir.isEmpty()){
                    f = new File(Dir.pop());
                    if(f.isFile()){
                        filelists.add(f.getAbsolutePath());
                    } else{
                        String arr[] = f.list();
                        try{
                            for(int i = 0;i<arr.length;i++){
                                Dir.add(f.getAbsolutePath()+"/"+arr[i]);
                            }
                        }
                        catch(NullPointerException exp){
                            Dir.remove(f.getAbsoluteFile());
                        }
                    }
                }
                
            } catch (IOException ex) {
                Logger.getLogger(StrUtil.class.getName()).log(Level.SEVERE, null, ex);
            }
            return filelists;
        }

    
    public static void CreateWindowsCharset(){
        HashSet<Integer> tmp = new HashSet<>();
        try {
            byte[] bytes = new byte[256];
            for (int i = 0; i <= 255; i++) {
                bytes[i] = (byte)i;
            }
            String newTest = new String(bytes, "Windows-1252");
            for (int i = 0; i < newTest.length(); i++) {
                if (newTest.codePointAt(i) != 65533) {
                    tmp.add(newTest.codePointAt(i));
                }
                else {
                    tmp.add(i);
                }
            }
            windowsCharset = Collections.unmodifiableSet(tmp);
        } catch (UnsupportedEncodingException ex) {
            Logger.getLogger(StrUtil.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
    public static String fixbrokenencoding(String s){
        try {
            byte[] windows = getWindowsBytes(s);
            if (windows == null) {
                return s;
            }
            String guess = new String(windows, "UTF-8");
            if (isGoodConversion(guess)) {
                return guess;
            }
            Logger.getLogger(StrUtil.class.getName()).log(Level.OFF, "Original: '{''}'{0}", s);
            return s;
        } catch (UnsupportedEncodingException ex) {
            Logger.getLogger(StrUtil.class.getName()).log(Level.SEVERE, null, ex);
        }
        return s;
    }
    private static byte[] getWindowsBytes(String s) {
        List<Byte> byteList = new ArrayList<>();
        for (int i = 0; i < s.length(); ) {
            int codePoint = s.codePointAt(i);
            if (windowsCharset.contains(codePoint)) {
                if (codePoint != 0x81 && codePoint != 0x8d && codePoint != 0x8f && codePoint != 0x90 && codePoint != 0x9d) {
                    try {
                        StringBuilder builder = new StringBuilder();
                        builder.appendCodePoint(codePoint);
                        for (byte b : builder.toString().getBytes("Windows-1252")) {
                            byteList.add(b);
                        }
                    } catch (UnsupportedEncodingException ex) {
                        Logger.getLogger(StrUtil.class.getName()).log(Level.SEVERE, null, ex);
                    }
                }
                else {
                    byteList.add((byte)codePoint);
                }
            }
            else {
                return null;
            }
            i += Character.charCount(codePoint);
        }
        byte[] bytes = new byte[byteList.size()];
        for (int i = 0; i < byteList.size(); i++) {
            bytes[i] = byteList.get(i);
        }
        return bytes;
    }
    public static boolean isGoodConversion(String s) {
        for (int i = 0; i < s.length(); ) {
            int codePoint = s.codePointAt(i);
            if (codePoint == 65533) {
                return false;
            }
            if (Character.getType(codePoint) == Character.SURROGATE) {
                return false;
            }
            i += Character.charCount(codePoint);
        }
        return true;
    }

 public static boolean validUTF8(byte[] input) {
      int i = 0;
      // Check for BOM
      if (input.length >= 3 && (input[0] & 0xFF) == 0xEF
        && (input[1] & 0xFF) == 0xBB & (input[2] & 0xFF) == 0xBF) {
       i = 3;
      }

      int end;
      for (int j = input.length; i < j; ++i) {
       int octet = input[i];
       if ((octet & 0x80) == 0) {
        continue; // ASCII
       }

       // Check for UTF-8 leading byte
       if ((octet & 0xE0) == 0xC0) {
        end = i + 1;
       } else if ((octet & 0xF0) == 0xE0) {
        end = i + 2;
       } else if ((octet & 0xF8) == 0xF0) {
        end = i + 3;
       } else {
        // Java only supports BMP so 3 is max
        return false;
       }

       while (i < end) {
        i++;
        octet = input[i];
        if ((octet & 0xC0) != 0x80) {
         // Not a valid trailing byte
         return false;
        }
       }
      }
      return true;
 }
    public static void Inserthashmap(HashMap<String,Integer> firstMap, HashMap<String,Integer> secondMap){
        Set<Map.Entry<String, Integer>> entries = firstMap.entrySet();
        for ( Map.Entry<String, Integer> entry : entries ) {
            Integer secondMapValue = secondMap.get( entry.getKey() );
            if ( secondMapValue == null ) {
                secondMap.put( entry.getKey(), entry.getValue() );
            }
            else {
                secondMap.put(entry.getKey(), entry.getValue()+secondMapValue);
            }
        }
    }
    public static int EnumerateTFIDF(HashMap<String,Integer> dict, long id, int featurenodenum,HashMap<String, Integer> featureids,HashMap<String, Integer> DF, BufferedWriter out, int N){
        Set<Map.Entry<String, Integer>> entries = dict.entrySet();
        int newfnum=featurenodenum;
        for ( Map.Entry<String, Integer> entry : entries ) {
            try {
                String word=entry.getKey();
                int tf=entry.getValue();
                int df=1;
                if(DF.containsKey(word)==true)
                    df=DF.get(word)+1;
                double tfidf=(double)tf*Math.log((double)N/df);
                long wid;
                if(featureids.containsKey(word)==false){
                    featureids.put(word, newfnum);
                    wid=newfnum;
                    newfnum++;
                }else{
                    wid=featureids.get(word);
                }
                out.write((id+1)+"\t"+(wid+1)+"\t"+tfidf+"\n");
            } catch (IOException ex) {
                Logger.getLogger(StrUtil.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
        return newfnum;
    }
    public static void printMap(Map mp, BufferedWriter out) {
        try {
            Iterator it = mp.entrySet().iterator();
            while (it.hasNext()) {
                try {
                    Map.Entry pairs = (Map.Entry)it.next();
                    out.write(pairs.getKey() + "\t" + pairs.getValue()+"\n");
                } catch (IOException ex) {
                    Logger.getLogger(StrUtil.class.getName()).log(Level.SEVERE, null, ex);
                }
            }
            out.close();
        } catch (IOException ex) {
            Logger.getLogger(StrUtil.class.getName()).log(Level.SEVERE, null, ex);
        }
    }

    public static void printMap(Map mp) {
        Iterator it = mp.entrySet().iterator();
        while (it.hasNext()) {
            Map.Entry pairs = (Map.Entry)it.next();
            System.out.print(pairs.getKey() + "\t" + pairs.getValue()+"\n");
        }
    }
    public static void printMap(Map mp, BufferedWriter out,double thres) {
        try {
            Iterator it = mp.entrySet().iterator();
            while (it.hasNext()) {
                try {
                    Map.Entry pairs = (Map.Entry)it.next();
                    double w=(double)pairs.getValue();
                    if(w>thres)
                        out.write(pairs.getKey() + "\t" + pairs.getValue()+"\n");
                } catch (IOException ex) {
                    Logger.getLogger(StrUtil.class.getName()).log(Level.SEVERE, null, ex);
                }
            }
            out.close();
        } catch (IOException ex) {
            Logger.getLogger(StrUtil.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
    public static void printMap(Map mp, BufferedWriter out, String []lists, double thres){
            Iterator it = mp.entrySet().iterator();
            while (it.hasNext()) {
                try {
                    Map.Entry pairs = (Map.Entry)it.next();
                    double w=(double)pairs.getValue();
                    String s=(String)pairs.getKey();
                    String []ids=s.split("\t");
                    if (ids.length<2)
                        continue;
                    int a=Integer.parseInt(ids[0]);
                    int b=Integer.parseInt(ids[1]);
                    if((lists[a-1]!=null&&lists[b-1]!=null&&lists[a-1].equals(lists[b-1]))||w>thres)
                        out.write(pairs.getKey() + "\t" + pairs.getValue()+"\n");
                } catch (IOException ex) {
                    Logger.getLogger(StrUtil.class.getName()).log(Level.SEVERE, null, ex);
                }
            } 
    }
      
    public static void printappendMap(Map mp, BufferedWriter out) {
    
            Iterator it = mp.entrySet().iterator();
            while (it.hasNext()) {
                try {
                    Map.Entry pairs = (Map.Entry)it.next();
                    out.write(pairs.getKey() + "\t" + pairs.getValue()+"\n");
                } catch (IOException ex) {
                    Logger.getLogger(StrUtil.class.getName()).log(Level.SEVERE, null, ex);
                }
            }
    }
     public static void printappendMapHTML(Map mp, BufferedWriter out) {
    
            Iterator it = mp.entrySet().iterator();
            while (it.hasNext()) {
                try {
                    Map.Entry pairs = (Map.Entry)it.next();
                    if(pairs.getValue().toString().length()>6)
                        out.write("<li>"+pairs.getValue().toString().substring(0, 6));
                    else
                        out.write("<li>"+pairs.getValue().toString());
                    out.write("\t\t|||\t\t" + pairs.getKey()+"</li>");
                } catch (IOException ex) {
                    Logger.getLogger(StrUtil.class.getName()).log(Level.SEVERE, null, ex);
                }
            }
    }
     public static void printappendMapHTML2(Map mp, BufferedWriter out) {
    
            Iterator it = mp.entrySet().iterator();
            while (it.hasNext()) {
                try {
                    Map.Entry pairs = (Map.Entry)it.next();
                    out.write("<blockquote><p>");
                    double count=(Double)pairs.getValue();
                    String content=pairs.getKey().toString();
                    if(content.contains("<br>")&&!content.contains("</br>")){
                        content.replaceAll("<br>", "");
                    }
                    if(count>1){
                        out.write("<span style=\"color:red\"</span>");
                        out.write("(Freq*Prob): "+pairs.getValue()+"\n");
                        
                        out.write(content+"</p></blockquote>");
                    }
                    else{
                        out.write("(Freq*Prob): "+pairs.getValue()+"\n");
                        out.write(content+"</p></blockquote>");
                    }
                } catch (IOException ex) {
                    Logger.getLogger(StrUtil.class.getName()).log(Level.SEVERE, null, ex);
                }
            }
    }
        public static void printpruneappendMapHTML(Map mp, BufferedWriter out) {
    
            Iterator it = mp.entrySet().iterator();
            while (it.hasNext()) {
                try {
                    Map.Entry pairs = (Map.Entry)it.next();
                    int count=(Integer)pairs.getValue();
                    if(count>1)
                        out.write("<li>"+pairs.getKey() + "\t" + pairs.getValue()+"</li>");
                } catch (IOException ex) {
                    Logger.getLogger(StrUtil.class.getName()).log(Level.SEVERE, null, ex);
                }
            }
    }
    public static int parseInt(String str){
		int value=0;
		
		if(str==null){
			return value;
		}
		
		if(str.trim().length()>0){
			try{
				value = Integer.parseInt(getDigitStr(str));
			}catch(Exception e){
				e.printStackTrace();
			}
		}	
		return value;
	}
	
    /**
     *
     * @param str
     * @return
     */
    public static long parseLong(String str){
		long value=0;
		
		if(str==null){
			return 0;
		}
		
		if(str.trim().length()>0){
			try{
				value = Long.parseLong(getDigitStr(str));
			}catch(Exception e){
				e.printStackTrace();
			}
		}	
		return value;
	}
	
        /**
         *
         * @param digStr
         * @return
         */
        public static String getDigitStr(String digStr){
		if(digStr==null){
			return "0";
		}
		StringBuffer sb=new StringBuffer();
		for(int i=0; i<digStr.length(); i++){
			char c=digStr.charAt(i);
			if(Character.isDigit(c)){
				sb.append(c);
			}
		}
		return sb.toString().trim();
	}

        /**
         *
         * @param searchTimeStr
         * @param queryRank
         * @param hitRank
         * @return
         */
        public static String getDocId(String searchTimeStr, int queryRank, int hitRank){
		StringBuffer sb= new StringBuffer();
		sb.append(searchTimeStr).append(".");
		if(queryRank<10){
			sb.append(0);
		}
		sb.append(queryRank).append(".");
		if(hitRank<10){
			sb.append(0);
		}
		sb.append(hitRank);
		return sb.toString().trim();
	}
        public static long parseDIGTime(String timestr){
            long timeIns = 0;
		DateFormat formatter = new SimpleDateFormat("yyyy-MM-dd");
		try {
			Date date = (Date) formatter.parse(timestr);
			timeIns = date.getTime() / 1000;
		} catch (Exception e) {
			e.printStackTrace();
		}
		return timeIns;
        }

        /**
         *
         * @param timeStr
         * @return
         */
        public static long parseSearchTime(String timeStr){
		long timeInMillis = 0;
		DateFormat formatter = new SimpleDateFormat("yyyyMMddHHmmss");
		try {
			Date date = (Date) formatter.parse(timeStr);
			timeInMillis = date.getTime() / 1000 * 1000;
		} catch (Exception e) {
			e.printStackTrace();
		}
		return timeInMillis;
	}
	
	
        /**
         *
         * @param timeLong
         * @return
         */
        public static String getTimeStr(long timeLong){
		Date date= new Date(timeLong);
		String timeStr = new SimpleDateFormat("yyyyMMddHHmmss").format(date);
		return timeStr;
	}
	
        /**
         *
         * @param arg
         */
        public static void main(String[] arg){
		//System.out.println(StrUtil.getSearchTime("20070811110001"));
		long start=StrUtil.parseSearchTime("20061108010001");
		long end=StrUtil.parseSearchTime("20071130220001");
		long number = (end - start)/(3*3600*1000);
		System.out.println(number*15);
		System.out.println((float)2978/number);
		System.out.println((float)44594/number/15);
		
	}
	
	
}
