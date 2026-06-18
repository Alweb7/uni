class BankAccount {
    private static int conti=0;
    private int id;
    private String nome_intestatario;
    private int liquidita;

    public BankAccount(String nome,int liquidita){
        id=conti;
        conti++;
        this.nome_intestatario=nome;
        this.liquidita=liquidita;
        
    }
    public BankAccount(String nome){
        id=conti;
        conti++;
        this.liquidita=0;
        this.nome_intestatario=nome;
    }
    public int getLiquidita(){
        return liquidita;
    }
    public String getNome_intestatario(){
        return nome_intestatario;
    }
    public void setNome_intestatario(String nome){
        this.nome_intestatario=nome;
    }
    public boolean withdraw(int prelievo){
        if (liquidita>=prelievo) {
            liquidita=liquidita-prelievo;
            return true;           
        }
        return false;
        
    }
    public static int getCounter(){
		return conti;
	}
    public void deposito(int deposito){
        liquidita=deposito+liquidita;
    }
    public int getId(){
        return id;
    }



}
public class TestBankAccount {
    public static void main(String[] args) {
        BankAccount b1 = new BankAccount("Pluto", 100);
		BankAccount b2 = new BankAccount("Paperino");
		System.out.println("Print Bank Account Info. Name: " + b1.getNome_intestatario() + " Balance: " + b1.getLiquidita());
		System.out.println("Print Bank Account Info. Name: " + b2.getNome_intestatario() + " Balance: " + b2.getLiquidita());
		System.out.println("Withdrawal Result "+ b1.withdraw(500));
		System.out.println("Print Bank Account Info After Withdrawal. Name: " + b2.getNome_intestatario() + " Balance: " + b2.getLiquidita());
		b2.deposito(800);
		System.out.println("Print Bank Account Info After Deposit. Name: " + b2.getNome_intestatario() + " Balance: " + b2.getLiquidita());
		b2.withdraw(200);
		System.out.println("Print Bank Account Info After Withdrawal of 200. Name: " + b2.getNome_intestatario() + " Balance: " + b2.getLiquidita());

		System.out.println("Number of Bank Accounts: " + BankAccount.getCounter());
    }
    
}