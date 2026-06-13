public class ArrayRest {
    static int[] array(){
        int[] array=new int[10];
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