public class ArrayRest {
    static int[] array(){
        int[] array = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        return array;
    }
    static void stampo_arry(int[] a){
        for (int i : a) {
            System.out.println(i);
        }
    }
    public static void main(String[] args) {
        int[] a;
        a=array();
        stampo_arry(a);

    }
}