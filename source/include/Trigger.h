#ifndef _TRIGGER_H_
#define _TRIGGER_H_

#define TRIGGER_ALWAYS  0
#define TRIGGER_LIFE    1
#define TRIGGER_TOTAL   2

#define TRIGGER_OPERATION_EQUAL         0
#define TRIGGER_OPERATION_LOWER_EQUAL   1
#define TRIGGER_OPERATION_GREATER_EQUAL 2
#define TRIGGER_OPERATION_TOTAL         3

#define TRIGGER_LOGIC_AND 0
#define TRIGGER_LOGIC_OR 1
#define TRIGGER_LOGIC_TOTAL 2


class Unit;

class Trigger
{
    public:
        static Trigger* copy(const Trigger *source);

        Trigger(int Type) : type(Type)
        {}
        virtual ~Trigger(){}

        virtual bool testTrigger(Unit *unit) = 0;

        int getType() const{
            return type;
        }

        int getValue() const{
            return value;
        }

        int getRelationalOperator() const{
            return relOperator;
        }

        void setValue(int newValue){
            value = newValue;
        }

        void setRelOperator(int newRel){
            relOperator = newRel;
        }

    protected:
        const int type;
        int value;
        int relOperator;

    private:
};

class TacticTrigger
{
    public:
        TacticTrigger(Trigger* A, Trigger* B, int oper);
        TacticTrigger(const TacticTrigger &source);
        ~TacticTrigger();

        bool testTrigger(Unit *unit);

        Trigger* getTriggerA();
        Trigger* getTriggerB();

        /// Tactic Trigger will own the Trigger
        void setTriggerA(Trigger* t);
        /// Tactic Trigger will own the Trigger
        void setTriggerB(Trigger* t);

        int getLogicOperator(){
            return operation;
        }

        void setLogicOperator(int newOperator)
        {
            operation = newOperator;
        }

    private:
        Trigger *tA, *tB;
        int operation;
};

//PORQUE O NOME DAS CLASSES ABAIXO ESTA FORA DO PADRAO?
class Trigger_Always : public Trigger
{
    public:
        Trigger_Always();
        ~Trigger_Always();

        bool testTrigger(Unit *unit);
};

class Trigger_Life : public Trigger
{
    public:
        Trigger_Life(int lifeTrigger, int trigOP);
        ~Trigger_Life();

        bool testTrigger(Unit *unit);
};

#endif // _TRIGGER_H_
