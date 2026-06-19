import java.util.Scanner;
public class Calcolatrice {
    int[] stack=new int[100];
    int top=0;
    public void push(int n){
        stack[top]=n;
        top++;
    }
    public int pop(){
        top--; // va messo prima perche dopo che fa il push fa su di uno [][2][] push 2--> pop fa ptima top che è all'ultimo top-- cosi va a quello prima se 
        if(top<0){ System.err.println("Errore"); return 0;}
        int val=stack[top];
        return val;
    }
    public int execution(String istruction){
        //int lunghezza=istruction.length();
        for(int i=0;i<istruction.length();i++){
            char c=istruction.charAt(i); //charAt() si usa per prender il char nella stringa in posizione i
            if (c>='0' &&  c<='9') { //possimao fare cosi perchè nella tabella ASCII 0=48 e 9=57 quindi 1,2,3,4,5,6,7,8 sono tra 48 e 57
                push(c-'0');    // cosi faccio pushsu 48-c= c pero int        
            }else if (c == '+') {
                int last    = pop();
                int secondLast = pop();
                push(secondLast + last);
            }
            else if (c == '*') {
                    int last    = pop();
                    int secondLast = pop();
                    push(secondLast * last);
            }
            else if (c == '/') {
                    int last    = pop();
                    int secondLast = pop();
                    push(secondLast / last);
            }
            else if (c == '%') {
                    int last    = pop();
                    int secondLast = pop();
                    push(secondLast%last);
            } 
            else if (c == '-') {
                    int last    = pop();
                    int secondLast = pop();
                    push(secondLast - last);
            }
            else if (c == '#') {
                PrintStack();
            }    
                    
        }
        return pop();
    }
    public void PrintStack(){
        for (int i = 0; i < top; i++) {
            System.out.print(stack[i] + " ");
        }
        
        System.out.println("]"+"lunghezza"+top); // Chiude la parentesi e va a capo
    }
}
class TestCalcolatrice {
    public static void main(String[] args) {
        Scanner s=new Scanner(System.in);
        System.out.printf("inserisci il numero:");
        String str=s.nextLine();  //come inserire una stringa nextInt(), nextDoubl()
        Calcolatrice c=new Calcolatrice();
        System.out.println("stampo risultato calcolo"+ c.execution(str));
    }

    
}   