public class ArrayMetodiIntEs {
    static int lunghezza(int[] a){
        int lunghezza;
        lunghezza=a.length;
        return lunghezza;
    }
    static void stampare(int[] a){
        for (int n : a) {
            System.out.println(n);
        }
    }
    static int somma(int[] a){
        int somma=0;
        for(int i=0;i<a.length;i++){
            somma+=a[i];
        }
        return somma;
    }
    static boolean confrontoDue(int[] a, int[] b){
    if (a.length != b.length) return false;
    for(int i = 0; i < a.length; i++){
        if (a[i] != b[i]) return false;
    }
    return true;
    }


    public static void main(String[] args) {

    }
}