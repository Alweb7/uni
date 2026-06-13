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
    static int max_value(int[] a) {
        int max = a[0];
        for (int i = 1; i < a.length; i++) {
            if (a[i] > max) max = a[i];
        }
        return max;
    }



    public static void main(String[] args) {
        int[] a = {3, 7, 1, 9, 4};
        int[] b = {3, 7, 1, 9, 4};
        int[] c = {3, 7, 1, 9, 5};
        int[] negativi = {-5, -2, -8};

        System.out.println("lunghezza: " + lunghezza(a));               // 5

        System.out.println("stampare:");
        stampare(a);                                                     // 3 7 1 9 4

        System.out.println("somma: " + somma(a));                       // 24

        System.out.println("confrontoDue a==b: " + confrontoDue(a, b)); // true
        System.out.println("confrontoDue a==c: " + confrontoDue(a, c)); // false

        System.out.println("find_number 9: " + find_number(a, 9));      // true
        System.out.println("find_number 0: " + find_number(a, 0));      // false

        System.out.println("max_value: " + max_value(a));               // 9
        System.out.println("max_value negativi: " + max_value(negativi)); // -2
    }
}