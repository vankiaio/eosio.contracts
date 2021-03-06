/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#pragma once

#include <eosiolib/asset.hpp>
#include <eosiolib/eosio.hpp>
#include <eosiolib/time.hpp>

#include <string>

namespace eosiosystem {
   class system_contract;
}

namespace eosio {

   using std::string;
   using eosio::permission_level;

   class [[eosio::contract("eosio.token")]] token : public contract {
      public:
         using contract::contract;

         [[eosio::action]]
         void create( name   issuer,
                      asset  maximum_supply);

         [[eosio::action]]
         void issue( name to, asset quantity, string memo );

         [[eosio::action]]
         void issuelock( name to, asset quantity, string memo, asset lockquantity, uint32_t unlock_delay_sec );

         [[eosio::action]]
         void retire( asset quantity, string memo );

         [[eosio::action]]
         void transfer( name    from,
                        name    to,
                        asset   quantity,
                        string  memo );

         [[eosio::action]]
         void open( name owner, const symbol& symbol, name ram_payer );

         [[eosio::action]]
         void close( name owner, const symbol& symbol );

         [[eosio::action]]
         void lock( name owner, asset quantity, uint32_t unlock_delay_sec );

         [[eosio::action]]      
         void unlock( name owner, symbol_code sym_code );

         [[eosio::action]]
         void dounlock( name owner, symbol_code sym_code );

         [[eosio::action]]
         void blacklistadd( name account, asset token_min );

         [[eosio::action]]
         void blacklistrm( name account );

         static asset get_supply( name token_contract_account, symbol_code sym_code )
         {
            stats statstable( token_contract_account, sym_code.raw() );
            const auto& st = statstable.get( sym_code.raw() );
            return st.supply;
         }

         static asset get_balance( name token_contract_account, name owner, symbol_code sym_code )
         {
            accounts accountstable( token_contract_account, owner.value );
            const auto& ac = accountstable.get( sym_code.raw() );
            return ac.balance;
         }

      private:
         struct [[eosio::table]] account {
            asset    balance;

            uint64_t primary_key()const { return balance.symbol.code().raw(); }
         };

         struct [[eosio::table]] currency_stats {
            asset    supply;
            asset    max_supply;
            name     issuer;

            uint64_t primary_key()const { return supply.symbol.code().raw(); }
         };

         struct frozen_balance {
            asset balance;
            uint32_t unlock_delay_sec;
            time_point_sec unlock_request_time;
            time_point_sec unlock_execute_time;
         };

         struct [[eosio::table]] locked_account {
            asset total_balance;
            std::vector<frozen_balance> balances;

            uint64_t primary_key()const { return total_balance.symbol.code().raw(); }
         };

         /// @abi table blacklist i64
         struct [[eosio::table]] blacklist_row {
            name account;
            asset tokens_blocked;

            uint64_t primary_key() const { return account.value; }
         };


         typedef eosio::multi_index< "accounts"_n, account > accounts;
         typedef eosio::multi_index< "stat"_n, currency_stats > stats;
         typedef eosio::multi_index< "locked"_n, locked_account > locked_accounts;
         typedef eosio::multi_index< "blacklist"_n, blacklist_row> blacklists;

         void sub_balance( name owner, asset value );
         void add_balance( name owner, asset value, name ram_payer );
         static bool sort_by_execute_time (frozen_balance i,frozen_balance j) {
            return (i.unlock_execute_time < j.unlock_execute_time);
         }
   };

} /// namespace eosio
