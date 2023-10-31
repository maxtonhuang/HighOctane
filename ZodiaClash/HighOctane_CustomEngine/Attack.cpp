#include "Attack.h"
#include "Battle.h"
#include "CharacterStats.h"
#include "AssetManager.h"
#include <random>

#include <rapidjson-master/include/rapidjson/document.h>
#include <rapidjson-master/include/rapidjson/writer.h>
#include <rapidjson-master/include/rapidjson/prettywriter.h>
#include <rapidjson-master/include/rapidjson/stringbuffer.h>
#include <rapidjson-master/include/rapidjson/istreamwrapper.h>

void Attack::UseAttack(CharacterStats* target) {
    CalculateDamage(*target);
    target->TakeDamage(damage);
}

void Attack::UseAttack(std::vector<CharacterStats*> target) {
    for (CharacterStats* t : target) {
        CalculateDamage(*t);
        t->TakeDamage(damage);
    }
}

void Attack::CalculateDamage(CharacterStats const& target)
{
    //attackerStats = owner.GetComponent<CharacterStats>();
    //targetStats = target.GetComponent<CharacterStats>();

    //critical hit chance
    static std::default_random_engine rng;
    static std::uniform_real_distribution<float> rand(0.f, 1.f);
    float randomValue = rand(rng);

    if (randomValue <= critRate)
    {
        //critical hit
        critCheck = true;

        damage = (std::max(minAttackMultiplier, maxAttackMultiplier) *
            ((float)skillAttackPercent / 100.f) * (owner->stats.attack * (100.f / (100.f + target.stats.defense)))
            * critMultiplier);
    }
    else
    {
        damage = (std::max(minAttackMultiplier, maxAttackMultiplier) *
            ((float)skillAttackPercent / 100.f) * (owner->stats.attack * (100.f / (100.f + target.stats.defense))));
    }
}

void Attack::SetOwner(CharacterStats* input) {
    owner = input;
}

std::vector<std::string> AttackList::GetAttackNames() {
    std::vector<std::string> output{};
    for (auto& a : data) {
        output.push_back(a.first + ".skill");
    }
    return output;
}

void AttackList::SaveAttack(Attack const& attack) {
    rapidjson::Document document;
    document.SetArray();
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    std::string attackPath{ assetmanager.GetDefaultPath() + "Skills/" + attack.attackName + ".skill" };
    rapidjson::Value object(rapidjson::kObjectType);

    rapidjson::Value nameValue;
    nameValue.SetString(attack.attackName.c_str(), static_cast<rapidjson::SizeType>(attack.attackName.length()), allocator);
    object.AddMember("Name", nameValue, allocator);
    object.AddMember("Type", (int)attack.attacktype, allocator);
    object.AddMember("Skill Attack", attack.skillAttackPercent, allocator);
    object.AddMember("Minimum Attack Multiplier", attack.minAttackMultiplier, allocator);
    object.AddMember("Maximum Attack Multiplier", attack.maxAttackMultiplier, allocator);
    object.AddMember("Crit Rate", attack.critRate, allocator);
    object.AddMember("Crit Multiplier", attack.critMultiplier, allocator);
    document.PushBack(object, allocator);

    // Save the JSON document to a file
    std::ofstream ofs(attackPath);
    if (ofs.is_open()) {
        rapidjson::StringBuffer buffer;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
        document.Accept(writer);
        ofs << buffer.GetString();
        ofs.close();
    }
}

void AttackList::LoadAttack(std::string attackPath) {
    Attack atk{};
    std::string atkname{};
    rapidjson::Document document;
    std::ifstream file(attackPath);
    
    if (!file.is_open()) {
        std::cout << "Failed to open file: " << attackPath << "\n";
        ASSERT(1, "Unable to open attack file!");
        return;
    }
    rapidjson::IStreamWrapper isw(file);
    document.ParseStream(isw);

    if (document.HasParseError()) {
        std::cerr << "Failed to parse attack file: " << attackPath << "\n";
        ASSERT(1, "Unable to parse attack file!");
        return;
    }

    for (rapidjson::SizeType i = 0; i < document.Size(); ++i) {
        const rapidjson::Value& mainObject = document[i];

        if (mainObject.HasMember("Name")) {
            const rapidjson::Value& object = mainObject["Name"];
            atkname = object.GetString();
            atk.attackName = atkname;
        }

        if (mainObject.HasMember("Type")) {
            const rapidjson::Value& object = mainObject["Type"];
            atk.attacktype = (AttackType)object.GetInt();
        }

        if (mainObject.HasMember("Skill Attack")) {
            const rapidjson::Value& object = mainObject["Skill Attack"];
            atk.skillAttackPercent = object.GetInt();
        }

        if (mainObject.HasMember("Minimum Attack Multiplier")) {
            const rapidjson::Value& object = mainObject["Minimum Attack Multiplier"];
            atk.minAttackMultiplier = object.GetFloat();
        }

        if (mainObject.HasMember("Maximum Attack Multiplier")) {
            const rapidjson::Value& object = mainObject["Maximum Attack Multiplier"];
            atk.maxAttackMultiplier = object.GetFloat();
        }

        if (mainObject.HasMember("Crit Rate")) {
            const rapidjson::Value& object = mainObject["Crit Rate"];
            atk.critRate = object.GetFloat();
        }

        if (mainObject.HasMember("Crit Multiplier")) {
            const rapidjson::Value& object = mainObject["Crit Multiplier"];
            atk.critMultiplier = object.GetFloat();
        }
    }
    data[atkname] = atk;
}
