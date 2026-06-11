public class ArrayMetodiIntEs {
    static int lunghezza(int[] a){
        return a.length;

    }
    static void stampare(int[] a){
        for (int i : a) {
            System.out.println(i);
        }

    }
    static int somma(int[] a){
        int somma=0;
        for (int i : a) {
            somma=somma+i;
        }
        return somma;

    }
    static boolean confrontoDue(int[] a,int[] b){
        boolean confronto=(a.length==b.length); // usao di case confronto sempre vero e faccio gia un controllo sul lenght
        int i=0;
        while (confronto && i<a.length) {
            confronto= (a[i]==b[i]); // facendo questo while ponco che confronto dovrà essere sicuramente vero cosi da permettermi di dire che è caso nela caso ci sia un false
            i++;            
        }
        return confronto;
    }
    static boolean trovoElemento(int[] a, int b){
        boolean trovato=false;
        for (int i : a) {
            if(i==b){
                trovato=true;
                break;
            }
        }
        return  trovato;
    }
    static int valoreMax(int[] a){
        int max=a[0];
        for (int i : a) {
            if(max<i){max=i;}
        }
        return max;
    }
    public static void main(String[] args) {
        
        int a[] = {-10, -60, -30};    	 
        System.out.println("lunghezza array = " + lunghezza(a));
        System.out.println("elementi: ");
        stampare(a);
        System.out.println("somma complessiva = " + somma(a));
        int b[] = {-10, -20, -30};    	 
        System.out.println(confrontoDue(a,b) ? "array uguali" : "array diversi");
        int v = -10;
        System.out.println(trovoElemento(a,v)?"valore " + v + " trovato" : "valore " + v + " non trovato");
        System.out.println("max valore " + valoreMax(a));
            
    }
}
