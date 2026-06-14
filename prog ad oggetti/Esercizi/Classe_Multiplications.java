public class Classe_Multiplications {
    static final int MAX = 10;

    public static int[][] Newarray(){
        int[][] a= new int[MAX][MAX];
        for(int i=0;i<MAX;i++){
            for(int j=0;j<MAX;j++){
                a[i][j]=(i+1)*(j+1);
            }
        }
        return a;
    }
    static void try_stamp(int[][] a){
        for (int[] is : a) {
            for (int n : is) {
                System.out.printf("%d",n);
            }
            System.out.println();
        }
    }
    static int prodotto(int b,int c){
        int[][] a=Newarray();
        int prod=a[b-1][c-1];
        return prod;
    }
    public static void main(String[] args) {
        int[][] tavola = Newarray();

        System.out.println("=== Tavola pitagorica ===");
        try_stamp(tavola);

        System.out.println("\n=== prodotto ===");
        System.out.println("3 x 4 = " + prodotto(3, 4));   // 12
        System.out.println("5 x 6 = " + prodotto(5, 6));   // 30
        System.out.println("7 x 7 = " + prodotto(7, 7));   // 49
        System.out.println("1 x 1 = " + prodotto(1, 1));   // 1
        System.out.println("10 x 10 = " + prodotto(10, 10)); // 100
    }
}
