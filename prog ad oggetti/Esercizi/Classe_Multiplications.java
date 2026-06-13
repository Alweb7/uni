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
                System.out.println(n);
            }
        }
    }
    public static void main(String[] args) {
        try_stamp(Newarray());
        
    }
}
