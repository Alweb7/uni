public class ArrayRest {
    public static int[] creaarray(){
        int[]a={1,2,3};
        return a;

    }
    static void stampa(int[] a){
        System.out.println("ecco gli elementi");
        for (int i : a) {
            System.out.println(i);
        }
    }
    public static void main(String[] args) {
        stampa(creaarray());
        
    }
    
}
