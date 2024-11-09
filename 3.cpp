#include <iostream>
#include <thread>
#include <future>

#define MAX 20
using namespace std;

class bankers
{
    private:
        int al[MAX][MAX],m[MAX][MAX],n[MAX][MAX],avail[MAX];
        int nop,nor,k,result[MAX],pnum,work[MAX],finish[MAX];

    public:
        bankers();
        void input();
        void method();
        int search(int);
        void display();
};

bankers::bankers()
{
    k=0;
    for(int i=0;i<MAX;i++)
    {
        for(int j=0;j<MAX;j++)
        {
            al[i][j]=0;
            m[i][j]=0;
            n[i][j]=0;
        }
        avail[i]=0;
        result[i]=0;
        finish[i]=0;
    }
}

void bankers::input()
{
    int i,j;
    cout << "Введите количество процессов:";
    cin >> nop;
    cout << "Введите количество ресурсов:";
    cin >> nor;
    cout << "Введите выделенные ресурсы для каждого процесса: " << endl;
    for(i=0;i<nop;i++)
    {
        cout<<"\nProcess "<<i;
        for(j=0;j<nor;j++)
        {
            cout<<"\nResource "<<j<<":";
            cin>>al[i][j];
        }
    }
    cout<<"Введите максимальное количество ресурсов, необходимых для каждого процесса: "<<endl;
    for(i=0;i<nop;i++)
    {
        cout<<"\nПроцесс "<<i;
        for(j=0;j<nor;j++)
        {
            cout<<"\nРесурс "<<j<<":";
            cin>>m[i][j];
            n[i][j]=m[i][j]-al[i][j];
        }
    }
    cout << "Введите доступные в настоящее время ресурсы в системе: ";
    for(j=0;j<nor;j++)
    {
        cout<<"Ресурс "<<j<<":";
        cin>>avail[j];
        work[j]=-1;
    }
    for(i=0;i<nop;i++)
        finish[i]=0;
}

void bankers::method()
{
    mutex mtx;

    int i=0,j,flag;
    while(1)
    {
        if(finish[i]==0)
        {
            pnum =search(i);
            if(pnum!=-1)
            {
                result[k++]=i;
                finish[i]=1;
                for(j=0;j<nor;j++)
                {
                    avail[j]=avail[j]+al[i][j];
                }
            }
        }
        i++;
        if(i==nop)
        {
            flag=0;
            for(j=0;j<nor;j++)
                if(avail[j]!=work[j])

            flag=1;
            for(j=0;j<nor;j++)
                work[j]=avail[j];

            if(flag==0)
                break;
            else
                i=0;
        }
    }

    mtx.unlock();
}

int bankers::search(int i)
{
    int j;
    for(j=0;j<nor;j++)
        if(n[i][j]>avail[j])
            return -1;
    return 0;
}

void bankers::display()
{
    int i,j;
    cout<<endl<<"ВЫВОД:";
    cout<<endl<<"========";
    cout<<endl<<"ПРОЦЕССУ\t ВЫДЕЛЕНО\t МАКСИМУМ\t ПОТРЕБНОСТИ";
    for(i=0;i<nop;i++)
    {
        cout<<"\nP"<<i+1<<"\t     ";
        for(j=0;j<nor;j++)
        {
            cout<<al[i][j]<<"  ";
        }
        cout<<"\t     ";
        for (j=0;j<nor;j++)
        {
            cout<<m[i][j]<<"  ";
        }
        cout<<"\t     ";
        for(j=0;j<nor;j++ )
        {
            cout<<n[i][j]<<"  ";
        }
    }
    cout<<"\nПоследовательность безопасных процессов такова: \n";
    for(i=0;i<k;i++)
    {
        int temp = result[i]+1 ;
        cout<<"P"<<temp<<" ";
    }
    int flg=0;
    for (i=0;i<nop;i++)
    {
        if(finish[i]==0)
        {
        flg=1;
        }
    }
    cout<<endl<<"RESULT:";
    cout<<endl<<"=======";
    if(flg==1)
        cout<<endl<<"Система находится в небезопасном состоянии и может произойти взаимоблокировка !!";
    else
        cout<<endl<<"Система находится в безопасном состоянии, и взаимоблокировки не произойдет !!";
}

int main()
{
    cout<<" АЛГОРИТМ ВЗАИМОБЛОКИРОВКИ БАНКИРОВ "<<endl;
    bankers B;
    B.input ( );

    auto startSingle = chrono::high_resolution_clock::now(); // таймер для начала
    B.method ( );
    B.display ( );
    auto endSingle = chrono::high_resolution_clock::now(); // таймер для конца
    chrono::duration<double> elapsedSingle = endSingle - startSingle; // находим разницу таймером в секундах

    cout << "\nПрограмма выполнилась за " << elapsedSingle.count() << " секунд\n";
}