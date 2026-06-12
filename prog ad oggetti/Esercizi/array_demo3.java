public class array_demo3 {
    private static int MAX_ARRAY=10;
    public static void main(String[] args) {
        int array[]=new int[MAX_ARRAY];
        for(int i=0; i<array.length; i++){
            array[i]=i+2;
        }
        for (int num : array) {
            System.out.println(num);
        }
    }
    
}
