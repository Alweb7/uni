public class Classe_Multiplications {
    static final int MAX = 10;

    public static int[] Newarray(){
        int[][] a= new int[MAX][MAX];
        for(int i=0;i<MAX;i++){
            for(int j=0;j<MAX;j++){
                a[i][j]=(i+1)*(j+1);
            }
        }
        return a;

    }
    public static void main(String[] args) {
        
    }
}
