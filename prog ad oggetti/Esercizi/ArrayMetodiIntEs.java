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
    static boolean find_number(int[] a,int n){
        
        for(int i = 0; i < a.length; i++){
            if (a[i]==n) {
                return true;
            }
        }
        return false;
    }
    static int max_value(int a[]){
        int max=0;
        for (int i : a) {
            if (i>max) {
                max=i;
            }
        }
        return max;
    }


    public static void main(String[] args) {

    }
}