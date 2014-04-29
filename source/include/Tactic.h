/*
 *  Strategos - Tactic.h
 *
 *  Esta classe apresenta diversos filhos que especificam uma certa regra
 *  a ser seguida no campo de batalha.
 *  Cada regra utiliza de parametros setados pelo jogador para que sejam
 *  ativadas. Uma vez ativada, uma regra liberara' uma action (atirar, tomar dano, etc.)
 *
 *	Last update: 06/07/2012
 */


#ifndef _TACTIC_H_
#define _TACTIC_H_

#include <vector>
#include <list>
#include <string>

#include "Action.h"
#include "Ship.h"
#include "Trigger.h"

#include "TacticValidationData.h"

#define TACTIC_ATTACK_NEAREST_ENEMY 0
#define TACTIC_ATTACK_WEAKEST_ENEMY 1
#define TACTIC_ATTACK_COLLAB        2
#define TACTIC_DEFENSE_COLAB        3
#define TACTIC_KAMIKASE             4
#define TACTIC_RETREAT              5
#define TACTIC_NUMBER               6
// Tatica especial
#define TACTIC_MOVE_RANDOM          100

class Unit;

struct TacticInfo
{
    TacticInfo(const Unit* unit)
        : allyUnit(unit)
    {
    }

    const Unit* allyUnit; // Unidade aliada para interagir
};

class Tactic
{
    protected:
        TacticTrigger tacticTrigger;
        TacticInfo info;
        const int type;

    public:
        static Tactic* copy(const Tactic *source);

        Tactic(const TacticInfo& Info, const TacticTrigger& Trigger, int Type)
            : tacticTrigger(Trigger), info(Info), type(Type)
        {}

        virtual ~Tactic() {
        }

        bool testTrigger(Unit *unit)
        {
            return tacticTrigger.testTrigger(unit);
        }

        int getType() const
        {
            return type;
        }

        void setInfo(const TacticInfo& info_)
        {
            info.allyUnit = info_.allyUnit;
        }

        const TacticInfo& getInfo() const
        {
            return info;
        }

        TacticTrigger& getTacticTrigger()
        {
            return tacticTrigger;
        }

        virtual void debugPrint() = 0;

        virtual std::string printTactic()
        {
            std::string str = "";

            //tipo da primeira trigger
            str.append(std::to_string(tacticTrigger.getTriggerA()->getType()));
            str.append(" ");
            //valor de comparacao
            str.append(std::to_string(tacticTrigger.getTriggerA()->getValue()));
            str.append(" ");
            //operador relacional
            str.append(std::to_string(tacticTrigger.getTriggerA()->getRelationalOperator()));
            str.append(" ");

            //operador logico
            str.append(std::to_string(tacticTrigger.getLogicOperator()));
            str.append(" ");

            //segunda trigger
            str.append(std::to_string(tacticTrigger.getTriggerB()->getType()));
            str.append(" ");
            str.append(std::to_string(tacticTrigger.getTriggerB()->getValue()));
            str.append(" ");
            str.append(std::to_string(tacticTrigger.getTriggerB()->getRelationalOperator()));
            str.append(" ");

            str.append(std::to_string(type));

            return str;
        }

        // Verifica se a tatica pode ser efetuada e nesse caso já adiciona uma acao para a nave
        // @return: 1 se a tatica foi validada, 0 caso contrario.
        virtual int validateTactic(std::list<Action*> &newActions, TacticValidationData& tvd ) = 0;
};

// Ataca o inimigo mais proximo dentro da RANGE da nave
// Nenhum parametro adicional
// Todos os esquadroes possuem esta tatica como ultima alternativa
class AttackNearestEnemy : public Tactic
{
    private:

    public:
        AttackNearestEnemy(const TacticInfo& info, const TacticTrigger& trigger);
        ~AttackNearestEnemy() {}

        void debugPrint() override {printf("AttackNearestEnemy");}

        int validateTactic(std::list<Action*> &newActions, TacticValidationData& tvd ) override;
};

// Ataca o inimigo mais fraco dentro da RANGE da nave
// Nenhum parametro adicional
class AttackWeakestEnemy : public Tactic
{
    private:

    public:
        AttackWeakestEnemy(const TacticInfo& info, const TacticTrigger& trigger);

        void debugPrint() override {printf("AttackWeakestEnemy");}

        int validateTactic(std::list<Action*> &newActions, TacticValidationData& tvd ) override;
};

// Ataque colaborativo
// Ataca o mesmo alvo que a unidade aliada estiver atacando
// Se a unidade aliada estiver morta essa tatica eh desconsiderada.
class AttackCollab : public Tactic
{
    private:

    public:
        AttackCollab(const TacticInfo& info, const TacticTrigger& trigger);
        ~AttackCollab(){}

        std::string printTactic();

        void debugPrint() override {printf("AttackCollab");}

        int validateTactic(std::list<Action*> &newActions, TacticValidationData& tvd ) override;
};

// Defende a unidade aliada
// Ataca quem estiver atacando a unidade aliada
class DefenseCollab : public Tactic
{
    private:

    public:
        DefenseCollab(const TacticInfo& info, const TacticTrigger& trigger);
        ~DefenseCollab(){};
        std::string printTactic();

        void debugPrint() override {printf("DefenseCollab");}

        int validateTactic(std::list<Action*> &newActions, TacticValidationData& tvd ) override;
};

// Suicidar-se na nave inimiga mais proxima
// Cada nave do conjunto eh testada para os triggers
// Aquelas que ativarem iram se atirar na nave inimiga mais proxima
// OBS: A unidade Mae NAO pode ter esta tatica
class Kamikase : public Tactic
{
    private:

    public:
        Kamikase(const TacticInfo& info, const TacticTrigger& trigger);

        void debugPrint() override {printf("Kamikase");}

        int validateTactic(std::list<Action*> &newActions, TacticValidationData& tvd ) override;
};

// Retornar para perto da unidade aliada
class Retreat : public Tactic
{
    private:
    public:
        Retreat(const TacticInfo& info, const TacticTrigger& trigger);

        std::string printTactic();

        void debugPrint() override {printf("Retreat");}

        int validateTactic(std::list<Action*> &newActions, TacticValidationData& tvd ) override;
};

// Mover aleatoriamente
// Todos os esquadroes possuem esta tatica como ultima alternativa
class MoveRandomly : public Tactic
{
    private:
        int cooldown; // Tempo para mudar de direcao

    public:
        MoveRandomly(const TacticInfo& info, const TacticTrigger& trigger);

        void debugPrint() override {printf("MoveRandomly");}

        int validateTactic(std::list<Action*> &newActions, TacticValidationData& tvd ) override;
};

#endif
