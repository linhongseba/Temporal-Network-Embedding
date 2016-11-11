package process;
import java.io.StringReader;
import java.util.ArrayList;

import org.apache.lucene.analysis.Analyzer;
import org.apache.lucene.analysis.TokenStream;
import org.apache.lucene.analysis.Token;

public class TokenProcessor {
	private static Analyzer kStemStdAnalyzer; 

	public TokenProcessor(){
		kStemStdAnalyzer = new KStemStandardAnalyzer();
	}
	
	public TokenStream getTokenStream(String inputStr){
		TokenStream stream = kStemStdAnalyzer.tokenStream("contents", new StringReader(inputStr));
		return stream;
	}
	public ArrayList<String> getTokenList(String inputStr) {
		ArrayList<String> res = new ArrayList<String> (1000);
		TokenStream stream = this.getTokenStream(inputStr);
		try{
			while (true) {
				Token token = stream.next();
				if (token == null) break;
				String s = new String(token.termBuffer(), 0, token.termLength());
				res.add(s);
			}
		}catch(Exception e) {
			e.printStackTrace();
		}
		return res;
	}
	public String getTokenString(String inputStr){
		StringBuffer sb = new StringBuffer();
		TokenStream stream = this.getTokenStream(inputStr);
		try{
			while (true) {
				Token token = stream.next();
				if (token == null) break;
				sb.append(token.termBuffer(), 0, token.termLength()).append(" ");
			}
		}catch(Exception e){
			e.printStackTrace();
		}
		return sb.toString().toLowerCase().trim();
	}
	
	public static void main(String[] arg){
		String input="www.ntu.edu.sg  ipods ipod ipod.s's iphone] [wii] Copyright � 2003, Center for Intelligent Information Retrieval,University of Massachusetts, Amherst. All rights reserved. University of Massachusetts must not be used to endorse or promote products derived from this software without prior written permission. To obtain permission, contact info@ciir.cs.umass.edu.";
		String input2="www.ntu.edu.sg  ipods ipod ipod.s's iphone] [wii] Copyright � 2003, Center for Intelligent Information Retrieval,University of Massachusetts, Amherst. All rights reserved. University of Massachusetts must not be used to endorse or promote products derived from this software without prior written permission. To obtain permission, contact info@ciir.cs.umass.edu.";
		String input3="www.ntu.edu.sg  ipods ipod ipod.s's iphone] [wii] Copyright � 2003, Center for Intelligent Information Retrieval,University of Massachusetts, Amherst. All rights reserved. University of Massachusetts must not be used to endorse or promote products derived from this software without prior written permission. To obtain permission, contact info@ciir.cs.umass.edu.";
                String input4="The AU was originally defined as the length of the semi-major axis of the Earth's elliptical orbit around the Sun. In 1976 the International Astronomical Union revised the definition of the AU for greater precision, defining it as that length for which the Gaussian gravitational constant (k) takes the value 0.017 202 098 95 when the units of measurement are the astronomical units of length, mass and time.[5][6][7] An equivalent definition is the radius of an unperturbed circular Newtonian orbit about the Sun of a particle having infinitesimal mass, moving with an angular frequency of 0.017 202 098 95 radians per day,[2] or that length for which the heliocentric gravitational constant (the product GM) is equal to (0.017 202 098 95)2 AU3/d2. It is approximately equal to the mean Earth-Sun distance.";
		String input5 = "上海";
		
		TokenProcessor tp = new TokenProcessor();
		System.out.println(tp.getTokenString(input));
		System.out.println(tp.getTokenString(input2));
		System.out.println(tp.getTokenString(input3));
                System.out.println(tp.getTokenString(input4));
                System.out.println(tp.getTokenString(input5));
		ArrayList<String>  res = tp.getTokenList(input);
		for (int i = 0; i < res.size(); i++) {
			System.out.println(res.get(i));
		}


	}
	
}
