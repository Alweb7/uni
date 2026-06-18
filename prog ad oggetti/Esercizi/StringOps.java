class StringOps {
    public static String longest(String[] s){
        assert s!=null && s.length>0 : "l'array s[] deve essere valido e non vuoto.";
        for(int i=0;i<s.length;i++){
            assert s[i]!=null :"L'array di stringe deve avere tutti valori validi";
        }
        String s_max=new String();
        for(int i=0;i<s.length;i++){
            if (s[i].length()>=s_max.length()) {
                  s_max=s[i];                        
            }
        }    
        return s_max;    
    }
    public static String concatAll(String[] s){
        assert s!=null && s.length>0 : "l'array s[] deve essere valido e non vuoto.";
        for(int i=0;i<s.length;i++){
            assert s[i]!=null :"L'array di stringe deve avere tutti valori validi";
        }
        String s_all=new String();
        for(int i=0;i<s.length;i++){
            s_all=s_all.concat(s[i]); //con il concat va sempre messa a qualcosa
        }
        return s_all;
    }
    public static String trim(String s){
        assert s != null : "la stringa in input non puo` essere null.";
        String s_no_spazzi=new String();
        int i=0;
        while (i < s.length() && s.charAt(i)==' ') {
            i++;            
        }
        int j=s.length()-1;
        while (j>=i && s.charAt(j)==' ') {
            j--;            
        }
        s_no_spazzi=s.substring(i,j+1); //esclude l'ultimo indice
        
        return s_no_spazzi;
    }
    public static void main(String[] args) {
        String[] arrayOfStrings = {"ciao", "     questa è una stringa      ", "questa è una stringa più lunga", "questa corta"};
        System.out.println("longest = " + longest(arrayOfStrings) + " ");
        System.out.println("concatAll = " + concatAll(arrayOfStrings) + " ");
        assert args.length > 0;
        System.out.println("trim(arrayOfStrings[1]) = " + trim(arrayOfStrings[1]) + " ");
    }
    
}
